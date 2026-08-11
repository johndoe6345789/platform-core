#pragma once

/**
 * @file cli_dispatch.h
 * @brief Dispatches parsed CLI sub-commands
 *        to their handler functions.
 */

#include "cli_options.h"
#include "users/backend/commands/create_admin.h"
#include "ecommerce/backend/commands/ecommerce_api.h"
#include "migration-runner/backend/commands/migrate.h"
#include "drogon-host/backend/commands/seed.h"
#include "drogon-host/backend/commands/serve.h"

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include <cstdint>
#include <cstdlib>
#include <string>

/**
 * @brief Run the matched sub-command.
 * @param migrateCmd Parsed migrate sub-command.
 * @param seedCmd    Parsed seed sub-command.
 * @param adminCmd   Parsed admin sub-command.
 * @param migrateOpts Migrate option values.
 * @param seedOpts    Seed option values.
 * @param adminOpts   Admin option values.
 * @param port  Serve port.
 * @param config Serve config path.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
inline int dispatchCommand(
    CLI::App* migrateCmd,
    CLI::App* seedCmd,
    CLI::App* adminCmd,
    CLI::App* ecommerceCmd,
    const MigrateOpts& migrateOpts,
    const SeedOpts& seedOpts,
    const AdminOpts& adminOpts,
    const EcommerceOpts& ecommerceOpts,
    std::uint16_t port,
    const std::string& config)
{
    try {
        if (*migrateCmd) {
            commands::cmdMigrate(
                migrateOpts.up,
                migrateOpts.down,
                migrateOpts.status);
        } else if (*seedCmd) {
            commands::cmdSeed(seedOpts.file);
        } else if (*adminCmd) {
            commands::cmdCreateAdmin(
                adminOpts.email,
                adminOpts.password);
        } else if (*ecommerceCmd) {
            commands::cmdEcommerceApi(
                ecommerceOpts.config);
        } else {
            commands::cmdServe(port, config);
        }
    } catch (const std::exception& ex) {
        spdlog::error(
            "Fatal: {}", ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
