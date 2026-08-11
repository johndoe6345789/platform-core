/**
 * @file migrate.cpp
 * @brief Implementation of the migrate sub-command.
 * @copyright 2024 Nextra Contributors
 */

#include "migrate.h"
#include "migrate_helpers.h"

#include "migration-runner/backend/MigrationRunner.h"
#include "migration-runner/backend/MigrationRollback.h"
#include "migration-runner/backend/MigrationStatusQuery.h"

#include <drogon/drogon.h>
#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace commands
{

namespace
{
auto okPrint()
{
    return [](const nlohmann::json& j) {
        fmt::print("{}\n", j.dump(2));
        drogon::app().quit();
    };
}
auto errQuit(std::string tag)
{
    return [tag](drogon::HttpStatusCode,
                 const std::string& msg) {
        spdlog::error("{}: {}", tag, msg);
        drogon::app().quit();
    };
}
} // anonymous namespace

static void doStatus(const std::string& cfg)
{
    runMigrateLoop([]{
        services::MigrationStatusQuery::getStatus(
            SERVICES_ROOT,
            okPrint(), errQuit("status"));
    }, cfg);
}

static void doDown(const std::string& cfg)
{
    runMigrateLoop([]{
        services::MigrationRollback::rollbackLast(
            SERVICES_ROOT,
            okPrint(), errQuit("rollback"));
    }, cfg);
}

static void doUp(const std::string& cfg)
{
    runMigrateLoop([]{
        services::MigrationRunner runner{
            SERVICES_ROOT, GRAPH_PATH, BOOTSTRAP_SQL};
        runner.runMigrations(
            okPrint(), errQuit("migrate up"));
    }, cfg);
}

void cmdMigrate(
    bool up, bool down, bool status,
    const std::string& config)
{
    if (status) {
        spdlog::info("Checking migration status...");
        doStatus(config);
        return;
    }
    if (down) {
        spdlog::info("Rolling back last migration...");
        doDown(config);
        return;
    }
    if (up) {
        spdlog::info("Applying pending migrations...");
        doUp(config);
        return;
    }
    fmt::print(
        "No action specified. "
        "Use --up, --down, or --status.\n");
}

} // namespace commands
