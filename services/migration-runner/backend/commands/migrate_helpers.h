#pragma once
/**
 * @file migrate_helpers.h
 * @brief Shared helpers for the migrate sub-command.
 */

#include <drogon/drogon.h>

#include <functional>

namespace commands
{

/** @brief Default path to the services/ root. */
constexpr auto SERVICES_ROOT = "services";

/** @brief Default path to migration-graph.json. */
constexpr auto GRAPH_PATH =
    "services/migration-graph.json";

/** @brief Default path to the bootstrap SQL. */
constexpr auto BOOTSTRAP_SQL =
    "services/migration-runner/migrations/"
    "001_schema_migrations_domain_column.sql";

/**
 * @brief Load Drogon config and run one async task,
 *        then call drogon::app().quit() inside @p task.
 * @param task   Callable queued into the event loop.
 * @param config Drogon JSON config file path.
 */
inline void runMigrateLoop(
    std::function<void()> task,
    const std::string& config = "config/config.json")
{
    drogon::app().loadConfigFile(config);
    drogon::app().getLoop()->queueInLoop(
        std::move(task));
    drogon::app().run();
}

} // namespace commands
