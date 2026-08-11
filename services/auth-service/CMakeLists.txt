## @brief nextra-auth — auth, users, api-keys,
##        user-preferences microservice.
cmake_minimum_required(VERSION 3.20)
project(nextra-auth VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(
    "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/service.cmake"
)

nextra_service(
    NAME nextra-auth
    DIRS
        auth/backend
        auth/backend/keycloak
        auth/backend/oauth
        auth/backend/passkeys
        auth/backend/totp
        auth/controllers
        users/backend
        users/backend/commands
        users/controllers
        api-keys/backend
        api-keys/controllers
        user-preferences/backend
        user-preferences/controllers
        email/backend
        badges/backend
        drogon-host/backend/commands
        search/events
)
