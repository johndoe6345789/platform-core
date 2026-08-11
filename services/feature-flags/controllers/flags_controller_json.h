#pragma once
/**
 * @file flags_controller_json.h
 * @brief JSON serialisation helpers for flag controllers.
 */

#include "feature-flags/backend/FlagTypes.h"

#include <nlohmann/json.hpp>

namespace controllers
{

/** @brief Convert a FeatureFlag value to its JSON form. */
inline nlohmann::json
flagToJson(const services::flags::FeatureFlag& f)
{
    return {
        {"id", f.id},
        {"key", f.key},
        {"description", f.description},
        {"enabled", f.enabled},
        {"rollout_pct", f.rolloutPct},
        {"targeting", f.targeting},
        {"updated_at", f.updatedAt},
    };
}

/** @brief Serialise a vector of flags into a JSON array. */
inline nlohmann::json flagsToJson(
    const std::vector<services::flags::FeatureFlag>& v)
{
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& f : v) arr.push_back(flagToJson(f));
    return arr;
}

/** @brief Parse a PATCH body into a FlagPatch value. */
inline services::flags::FlagPatch
parseFlagPatch(const nlohmann::json& body)
{
    services::flags::FlagPatch p;
    if (body.contains("description")
        && body["description"].is_string())
        p.description = body["description"]
                            .get<std::string>();
    if (body.contains("enabled")
        && body["enabled"].is_boolean())
        p.enabled = body["enabled"].get<bool>();
    if (body.contains("rollout_pct")
        && body["rollout_pct"].is_number_integer()) {
        auto v = body["rollout_pct"].get<int>();
        if (v < 0) v = 0;
        if (v > 100) v = 100;
        p.rolloutPct = static_cast<std::int16_t>(v);
    }
    if (body.contains("targeting")
        && body["targeting"].is_object())
        p.targeting = body["targeting"];
    return p;
}

} // namespace controllers
