/**
 * @file seed_feature_toggles.cpp
 * @brief Feature toggle seed loading.
 */

#include "seed_handlers.h"
#include "seed_io.h"

#include <string>

namespace commands::seed
{

void loadFeatureToggles(
    const std::filesystem::path& path,
    const drogon::orm::DbClientPtr& db)
{
    for (const auto& flag : readSeedArray(path)) {
        db->execSqlSync(
            "INSERT INTO feature_toggles"
            " (key, enabled, description, scope, created_at, updated_at)"
            " VALUES ($1,$2,$3,$4,NOW(),NOW())"
            " ON CONFLICT (key) DO UPDATE SET"
            " enabled = EXCLUDED.enabled,"
            " description = EXCLUDED.description,"
            " scope = EXCLUDED.scope,"
            " updated_at = NOW()",
            flag.at("key").get<std::string>(),
            flag.value("enabled", false),
            flag.value("description", std::string{}),
            flag.value("scope", "global"));
    }
}

} // namespace commands::seed
