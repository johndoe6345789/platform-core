/**
 * @file MigrationStateStore.cpp
 * @brief schema_migrations table management implementation.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto MigrationStateStore::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

void MigrationStateStore::ensureTable(std::function<void()> then,
                                      ErrCallback onError)
{
    auto dbClient = db();
    const std::string check =
        "SELECT 1 FROM information_schema.tables "
        "WHERE table_schema='public' "
        "  AND table_name='schema_migrations'";
    *dbClient << check
        >> [dbClient, then, onError](const Result& r) {
            if (!r.empty()) { then(); return; }
            const std::string create = R"(
                CREATE TABLE schema_migrations (
                    id         SERIAL PRIMARY KEY,
                    filename   TEXT UNIQUE NOT NULL,
                    applied_at TIMESTAMPTZ NOT NULL
                               DEFAULT NOW()
                )
            )";
            *dbClient << create
                >> [then](const Result&) { then(); }
                >> [onError](const DrogonDbException& e) {
                    spdlog::error("ensureTable: {}",
                                  e.base().what());
                    onError(k500InternalServerError,
                            "Failed to create "
                            "schema_migrations");
                };
        }
        >> [onError](const DrogonDbException& e) {
            spdlog::error("ensureTable check: {}",
                          e.base().what());
            onError(k500InternalServerError,
                    "Failed to check "
                    "schema_migrations");
        };
}

} // namespace services
