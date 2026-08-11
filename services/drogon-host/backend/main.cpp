/**
 * @file main.cpp
 * @brief Entry point for the nextra-api service.
 *
 * Parses CLI args via CLI11, then dispatches to the
 * matching sub-command handler.
 *
 * @copyright 2024 Nextra Contributors
 */

#include "cli_daemon_opts.h"
#include "cli_dispatch.h"
#include "cli_dispatch_daemons.h"
#include "cli_setup.h"
#include "cli_setup_daemons.h"
#include "audit/backend/commands/audit_manager.h"

#include <cstdint>
#include <cstdlib>
#include <string>

/**
 * @brief Application entry point.
 * @param argc Argument count.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS or EXIT_FAILURE.
 */
int main(int argc, char* argv[])
{
    CLI::App app{
        "nextra-api -- backend service"};
    app.require_subcommand(0, 1);

    // -- serve (default) --
    std::uint16_t servePort{8080};
    std::string serveConfig{
        "config/config.json"};
    auto* serveCmd = app.add_subcommand(
        "serve", "Start the HTTP server");
    serveCmd->add_option(
        "-p,--port", servePort,
        "TCP port to listen on")
        ->default_val(8080);
    serveCmd->add_option(
        "-c,--config", serveConfig,
        "Path to Drogon JSON config")
        ->default_val("config/config.json");

    MigrateOpts migrateOpts;
    SeedOpts seedOpts;
    AdminOpts adminOpts;
    EcommerceOpts ecommerceOpts;
    DaemonOpts daemonOpts;
    DaemonCmds daemonCmds;
    auto* migrateCmd = addMigrateCmd(app, migrateOpts);
    auto* seedCmd = addSeedCmd(app, seedOpts);
    auto* adminCmd = addAdminCmd(app, adminOpts);
    auto* ecommerceCmd =
        addEcommerceCmd(app, ecommerceOpts);
    addDaemonCmds(app, daemonOpts, daemonCmds);

    // -- audit-manager (Phase 1.2 Kafka consumer) --
    std::string auditConfig{"config/config.json"};
    auto* auditCmd = app.add_subcommand(
        "audit-manager",
        "Run the hash-chained audit daemon");
    auditCmd->add_option(
        "-c,--config", auditConfig,
        "Path to Drogon JSON config")
        ->default_val("config/config.json");

    CLI11_PARSE(app, argc, argv);

    if (*auditCmd) {
        try {
            commands::cmdAuditManager(auditConfig);
            return EXIT_SUCCESS;
        } catch (const std::exception& ex) {
            spdlog::error(
                "audit-manager: {}", ex.what());
            return EXIT_FAILURE;
        }
    }

    int daemonRc{EXIT_SUCCESS};
    if (dispatchDaemonCommand(
            daemonCmds, daemonOpts, daemonRc)) {
        return daemonRc;
    }

    return dispatchCommand(
        migrateCmd, seedCmd, adminCmd, ecommerceCmd,
        migrateOpts, seedOpts, adminOpts, ecommerceOpts,
        servePort, serveConfig);
}
