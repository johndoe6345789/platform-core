## @brief nextra-platform — feature flags, i18n,
##        rate-limit, portal, api-docs.
cmake_minimum_required(VERSION 3.20)
project(nextra-platform VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/service.cmake"
)

# Generate portal_embedded.h from tool-links.json + styles.css
set(_proj_root "${CMAKE_CURRENT_SOURCE_DIR}/../..")
file(READ "${_proj_root}/shared/constants/tool-links.json"
    NEXTRA_TOOL_LINKS_JSON)
file(READ
    "${_proj_root}/services/portal/static/styles.css"
    NEXTRA_PORTAL_STYLES_CSS)
file(MAKE_DIRECTORY
    "${CMAKE_BINARY_DIR}/generated/portal/backend")
configure_file(
    "${_proj_root}/services/portal/backend/portal_embedded.h.in"
    "${CMAKE_BINARY_DIR}/generated/portal/backend/portal_embedded.h"
    @ONLY)

nextra_service(
    NAME nextra-platform
    DIRS
        feature-flags/backend
        feature-flags/controllers
        i18n/backend
        i18n/controllers
        rate-limit/backend
        rate-limit/controllers
        portal/backend
        portal/controllers
        drogon-host/controllers
        api-documentation/backend
        email/backend
        elasticsearch/backend
        auth/backend
)

target_include_directories(nextra-platform
    PRIVATE "${CMAKE_BINARY_DIR}/generated")
