## @brief nextra-infra — webhooks, cron, backup,
##        job-queue, email, imap-sync.
cmake_minimum_required(VERSION 3.20)
project(nextra-infra VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/service.cmake"
)

nextra_service(
    NAME nextra-infra
    DIRS
        webhooks/backend
        webhooks/backend/commands
        webhooks/controllers
        cron/backend
        cron/backend/commands
        cron/controllers
        backup/backend
        backup/backend/commands
        backup/controllers
        job-queue/backend
        job-queue/backend/commands
        job-queue/controllers
        email/backend
        email/controllers
        imap-sync/backend
        blog/backend
        auth/backend
)
