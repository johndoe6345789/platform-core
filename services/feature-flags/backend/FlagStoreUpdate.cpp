/**
 * @file FlagStoreUpdate.cpp
 * @brief Update flag rows + emit audit trail entries.
 */

#include "FlagStore.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

using namespace drogon;
using namespace drogon::orm;

namespace services::flags
{

static nlohmann::json flagToJson(const FeatureFlag& f)
{
    return {
        {"description", f.description},
        {"enabled", f.enabled},
        {"rollout_pct", f.rolloutPct},
        {"targeting", f.targeting},
    };
}

std::optional<FeatureFlag>
FlagStore::update(const std::string& key,
                  const FlagPatch& patch,
                  const std::string& actorId)
{
    auto existing = findByKey(key);
    if (!existing) return std::nullopt;

    FeatureFlag next = *existing;
    if (patch.description)
        next.description = *patch.description;
    if (patch.enabled) next.enabled = *patch.enabled;
    if (patch.rolloutPct)
        next.rolloutPct = *patch.rolloutPct;
    if (patch.targeting) next.targeting = *patch.targeting;

    auto db = app().getDbClient();
    auto r = db->execSqlSync(
        "UPDATE feature_flags SET description=$1, "
        "enabled=$2, rollout_pct=$3, targeting=$4::jsonb, "
        "updated_at=now() WHERE key=$5 "
        "RETURNING id, key, description, enabled, "
        "rollout_pct, targeting::text AS targeting, "
        "updated_at::text AS updated_at",
        next.description, next.enabled,
        static_cast<int>(next.rolloutPct),
        next.targeting.dump(), key);
    if (r.empty()) return std::nullopt;

    auto prev = flagToJson(*existing).dump();
    auto nxt = flagToJson(next).dump();
    try {
        if (actorId.empty()) {
            db->execSqlSync(
                "INSERT INTO feature_flag_audit"
                "(flag_id, prev, next) VALUES "
                "($1, $2::jsonb, $3::jsonb)",
                existing->id, prev, nxt);
        } else {
            db->execSqlSync(
                "INSERT INTO feature_flag_audit"
                "(flag_id, actor_id, prev, next) VALUES "
                "($1, $2::uuid, $3::jsonb, $4::jsonb)",
                existing->id, actorId, prev, nxt);
        }
    } catch (const DrogonDbException& e) {
        spdlog::error("flag audit insert: {}",
                      e.base().what());
    }
    return rowToFlag(r[0]);
}

} // namespace services::flags
