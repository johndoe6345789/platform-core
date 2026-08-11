# syntax=docker/dockerfile:1.7
# Backups frontend — monorepo-aware build.
# Contexts: default=services/backup/admin, shared=shared

# -- Stage 1: deps --------------------------
FROM node:20-alpine AS deps
ARG NPM_REGISTRY=https://registry.npmjs.org
ENV NPM_CONFIG_REGISTRY=$NPM_REGISTRY
COPY --from=shared . /build/shared/
WORKDIR /build/services/backup/admin
COPY package.json package-lock.json* ./
RUN npm config set fetch-retries 5 \
  && npm config set fetch-retry-mintimeout 20000 \
  && npm config set fetch-retry-maxtimeout 120000
RUN npm install --prefer-offline --no-audit --no-fund
RUN ln -s \
    /build/services/backup/admin/node_modules \
    /build/node_modules

# -- Stage 2: build -------------------------
FROM deps AS build
ARG NEXT_BASE_PATH=/backups
WORKDIR /build/services/backup/admin
COPY . .
ENV NEXT_TELEMETRY_DISABLED=1
ENV NEXT_BASE_PATH=${NEXT_BASE_PATH}
RUN mkdir -p public
RUN npm run build

# -- Stage 3: runner -------------------------
FROM node:22-bookworm-slim AS runner
WORKDIR /app
ENV NODE_ENV=production
ENV NEXT_TELEMETRY_DISABLED=1
RUN groupadd --system --gid 1001 nodejs && \
    useradd --system --uid 1001 nextjs
COPY --from=build --chown=nextjs:nodejs \
    /build/services/backup/admin/.next/standalone ./
COPY --from=build --chown=nextjs:nodejs \
    /build/services/backup/admin/public \
    ./services/backup/admin/public
COPY --from=build --chown=nextjs:nodejs \
    /build/services/backup/admin/.next/static \
    ./services/backup/admin/.next/static
USER nextjs
EXPOSE 3000
ENV PORT=3000
ENV HOSTNAME="0.0.0.0"
WORKDIR /app/services/backup/admin
CMD ["node", "server.js"]
