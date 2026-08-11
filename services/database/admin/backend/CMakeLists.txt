cmake_minimum_required(VERSION 3.20)
project(pgadmin-backend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Drogon REQUIRED)
find_package(OpenSSL REQUIRED)

add_executable(pgadmin-server
    src/main.cpp
    src/controllers/HealthCtrl.cpp
    src/controllers/AuthCtrl.cpp
    src/controllers/TableCtrl.cpp
    src/controllers/TableDataCtrl.cpp
    src/controllers/TableSchemaCtrl.cpp
    src/controllers/TableMutCtrl.cpp
    src/controllers/ColumnCtrl.cpp
    src/controllers/ColumnMutCtrl.cpp
    src/controllers/ConstraintCtrl.cpp
    src/controllers/ConstraintMutCtrl.cpp
    src/controllers/IndexCtrl.cpp
    src/controllers/IndexMutCtrl.cpp
    src/controllers/QueryCtrl.cpp
    src/controllers/RecordCtrl.cpp
    src/controllers/RecordUpdateCtrl.cpp
    src/filters/SessionFilter.cpp
    src/services/SqlValidator.cpp
    src/services/SessionService.cpp
    src/services/QueryBuilder.cpp
)

target_include_directories(pgadmin-server
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(pgadmin-server
    PRIVATE
        Drogon::Drogon
        OpenSSL::SSL
        OpenSSL::Crypto
)

install(TARGETS pgadmin-server
    RUNTIME DESTINATION bin
)
