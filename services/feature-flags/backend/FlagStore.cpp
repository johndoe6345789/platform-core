/**
 * @file FlagStore.cpp
 * @brief Synchronous CRUD over the feature_flags table.
 */

#include "FlagStore.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

using namespace drogon;
using namespace drogon::orm;

namespace services::flags
{

FeatureFlag FlagStore::rowToFlag(const Row& row) const
{
    FeatureFlag f;
    f.id = row["id"].as<std::int64_t>();
    f.key = row["key"].as<std::string>();
    f.description = row["description"].as<std::string>();
    f.enabled = row["enabled"].as<bool>();
    f.rolloutPct = row["rollout_pct"].as<std::int16_t>();
    f.targeting = nlohmann::json::parse(
        row["targeting"].as<std::string>(), nullptr, false);
    if (f.targeting.is_discarded())
        f.targeting = nlohmann::json::object();
    f.updatedAt = row["updated_at"].as<std::string>();
    return f;
}

std::vector<FeatureFlag> FlagStore::listAll()
{
    std::vector<FeatureFlag> out;
    auto db = app().getDbClient();
    auto r = db->execSqlSync(
        "SELECT id, key, description, enabled, "
        "rollout_pct, targeting::text AS targeting, "
        "updated_at::text AS updated_at "
        "FROM feature_flags ORDER BY key");
    out.reserve(r.size());
    for (const auto& row : r) out.push_back(rowToFlag(row));
    return out;
}

std::optional<FeatureFlag>
FlagStore::findByKey(const std::string& key)
{
    auto db = app().getDbClient();
    auto r = db->execSqlSync(
        "SELECT id, key, description, enabled, "
        "rollout_pct, targeting::text AS targeting, "
        "updated_at::text AS updated_at "
        "FROM feature_flags WHERE key = $1",
        key);
    if (r.empty()) return std::nullopt;
    return rowToFlag(r[0]);
}

nlohmann::json
FlagStore::audit(std::int64_t flagId, int limit)
{
    auto db = app().getDbClient();
    auto r = db->execSqlSync(
        "SELECT actor_id::text AS actor_id, "
        "prev::text AS prev, next::text AS next, "
        "at::text AS at FROM feature_flag_audit "
        "WHERE flag_id = $1 ORDER BY at DESC LIMIT $2",
        flagId, limit);
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& row : r) {
        arr.push_back({
            {"actor",
             row["actor_id"].isNull()
                 ? "" : row["actor_id"].as<std::string>()},
            {"prev", nlohmann::json::parse(
                 row["prev"].as<std::string>(),
                 nullptr, false)},
            {"next", nlohmann::json::parse(
                 row["next"].as<std::string>(),
                 nullptr, false)},
            {"at", row["at"].as<std::string>()},
        });
    }
    return arr;
}

} // namespace services::flags
