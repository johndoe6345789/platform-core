cmake_minimum_required(VERSION 3.20)
project(manager LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(CLI11 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(fmt REQUIRED)
find_package(inja REQUIRED)

add_executable(manager
    main.cpp
    commands/ActCmd.cpp
    commands/ActCmdReg.cpp
    commands/ActCmdRun.cpp
    commands/BuildCmd.cpp
    commands/CmakeGenCmd.cpp
    commands/CmakeGenCtx.cpp
    commands/ConanInstallCmd.cpp
    commands/DockerBuildxCmd.cpp
    commands/DockerCmd.cpp
    commands/DockerDepsCmd.cpp
    commands/DockerStatusCmd.cpp
    commands/ElasticCmd.cpp
    commands/ElasticCmdDrop.cpp
    commands/ElasticCmdInit.cpp
    commands/ElasticCmdReindex.cpp
    commands/ElasticCmdStatus.cpp
    commands/GenerateCmd.cpp
    commands/LintCmd.cpp
    commands/LintCmdFmt.cpp
    commands/PackageRepoCmd.cpp
    commands/PackageRepoCmdHelpers.cpp
    commands/PackageRepoCmdPull.cpp
    commands/PackageRepoCmdReg.cpp
    commands/S3Cmd.cpp
    commands/S3CmdUp.cpp
    commands/SeedTransCmd.cpp
    commands/SeedTransGen.cpp
    commands/SetupExoticArchCmd.cpp
    commands/TestCmd.cpp
    commands/WorkflowCtx.cpp
    commands/WorkflowExec.cpp
    commands/WorkflowLoad.cpp
    commands/WorkflowReg.cpp
    commands/WorkflowSteps.cpp
)

target_include_directories(manager
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(manager
    PRIVATE
        CLI11::CLI11
        nlohmann_json::nlohmann_json
        fmt::fmt
        pantor::inja
)

install(TARGETS manager RUNTIME DESTINATION bin)
