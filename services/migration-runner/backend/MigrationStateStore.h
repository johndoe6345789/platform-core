#pragma once
/**
 * @file MigrationStateStore.h
 * @brief Shared migration types and schema-migrations table manager.
 *
 * Defines the common callback aliases and MigrationStatus type used
 * across all migration modules, and exposes ensureTable() which
 * guarantees the tracking table exists before any query runs.
 */

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

#include <functional>
#include <string>

namespace services
{

using json = nlohmann::json;
using DbClientPtr = drogon::orm::DbClientPtr;
using Callback = std::function<void(json)>;
using ErrCallback = std::function<void(drogon::HttpStatusCode, std::string)>;

/**
 * @brief Status of one migration file.
 */
struct MigrationStatus {
    std::string filename;
    std::string appliedAt; ///< ISO timestamp, or empty if pending.
    bool pending{true};
};

/**
 * @class MigrationStateStore
 * @brief Manages the `schema_migrations` tracking table.
 */
class MigrationStateStore
{
  public:
    /**
     * @brief Ensure the `schema_migrations` table exists.
     *
     * Creates the table if it is absent, then invokes @p then.
     * On failure @p onError is called instead.
     *
     * @param then    Continuation to run after the table is ready.
     * @param onError Error callback.
     */
    static void ensureTable(std::function<void()> then, ErrCallback onError);

    /**
     * @brief Convenience accessor for the default DB client.
     *
     * @return Shared pointer to the Drogon ORM DB client.
     */
    [[nodiscard]] static auto db() -> DbClientPtr;
};

} // namespace services
