## @brief nextra-migrate — one-shot DB migration runner.
##
## Replaces the monolith `nextra-api migrate` sidecar: a
## standalone binary that applies all pending migrations and
## exits, so the monolith can be retired. Pulls the
## migration-runner (not in NEXTRA_SERVICE_SHARED_DIRS) via DIRS.
cmake_minimum_required(VERSION 3.20)
project(nextra-migrate VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/service.cmake"
)

nextra_service(
    NAME nextra-migrate
    NO_HTTP
    DIRS
        migration-runner/backend
        migration-runner/backend/commands
)
