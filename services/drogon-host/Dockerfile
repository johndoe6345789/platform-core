# syntax=docker/dockerfile:1.7
ARG DEPS_IMAGE=debian:sid
ARG RUNTIME_IMAGE=debian:sid-slim

FROM ${DEPS_IMAGE} AS build

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    bison \
    ca-certificates \
    cmake \
    flex \
    g++-13 \
    gcc-13 \
    git \
    libpq-dev \
    make \
    ninja-build \
    pkg-config \
    python3 \
    python3-pip && \
    rm -rf /var/lib/apt/lists/*

RUN ln -sf /usr/bin/gcc-13 /usr/local/bin/cc && \
    ln -sf /usr/bin/gcc-13 /usr/local/bin/gcc && \
    ln -sf /usr/bin/g++-13 /usr/local/bin/c++ && \
    ln -sf /usr/bin/g++-13 /usr/local/bin/g++

RUN python3 -m pip install --break-system-packages \
    --no-cache-dir "conan>=2.0,<3.0"

WORKDIR /src
COPY . .

ENV CONAN_SKIP_TEST=1
ENV CC=gcc-13
ENV CXX=g++-13

RUN cd /src/services/drogon-host && \
    conan profile detect --force && \
    conan install . --build=missing \
    --output-folder=build \
    -s build_type=Release && \
    cd /src && \
    cmake -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=services/drogon-host/build/build/Release/generators/conan_toolchain.cmake && \
    cmake --build build -j"$(nproc)"

# ── Extract migration SQL files only ──────────────────────────────
# Mirrors docker/nextra-api.Dockerfile: copies only *.sql from
# services/<domain>/migrations/ so the runtime image can run
# `nextra-api migrate` without bundling all C++ source.
FROM build AS migration-files

RUN find /src/services -type f -name "*.sql" \
        -path "*/migrations/*" | \
    while read -r f; do \
        rel="${f#/src/}"; \
        mkdir -p "/out/$(dirname "$rel")"; \
        cp "$f" "/out/$rel"; \
    done

FROM ${RUNTIME_IMAGE} AS runtime

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ca-certificates \
    libpq5 \
    libssl3 && \
    rm -rf /var/lib/apt/lists/*

RUN groupadd --system nextra && \
    useradd --system --gid nextra --create-home nextra

WORKDIR /app

COPY --from=build /src/build/nextra-api ./nextra-api
COPY services/drogon-host/config ./config
COPY --from=build /src/services/audit/constants.json \
    ./constants/audit-manager.json
COPY --from=build /src/services/backup/constants.json \
    ./constants/backup-manager.json
COPY --from=build /src/services/cron/constants.json \
    ./constants/cron-manager.json
COPY --from=build /src/services/ecommerce/constants.json \
    ./constants/ecommerce.json
COPY --from=build /src/services/image/constants.json \
    ./constants/image-processor.json
COPY --from=build /src/services/job-queue/constants.json \
    ./constants/job-scheduler.json
COPY --from=build /src/services/notifications/constants/notifications.json \
    ./constants/notification-router.json
COPY --from=build /src/services/pdf/constants.json \
    ./constants/pdf-generator.json
COPY --from=build /src/services/search/constants.json \
    ./constants/search-indexer.json
COPY --from=build /src/services/streaming/constants.json \
    ./constants/media-streaming.json
COPY --from=build /src/services/video/constants.json \
    ./constants/video-transcoder.json
COPY --from=build /src/services/webhooks/constants.json \
    ./constants/webhook-dispatcher.json
COPY --from=build \
    /src/services/http-filters/constants/security-headers.json \
    ./constants/security-headers.json

# Migration SQL files + dependency graph (needed by `nextra-api
# migrate`) — without these the migrate sidecar cannot bootstrap.
COPY --from=migration-files /out/services ./services
COPY services/migration-graph.json \
    ./services/migration-graph.json

RUN mkdir -p logs public uploads && \
    chown -R nextra:nextra /app

USER nextra

EXPOSE 8080

ENTRYPOINT ["./nextra-api"]
CMD ["serve", "--config", "config/config.prod.json"]
