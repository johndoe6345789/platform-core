#pragma once
/**
 * @file FlagTypes.h
 * @brief Value types for the feature flags service.
 */

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <cstdint>

namespace services::flags
{

/** In-memory representation of a feature flag row. */
struct FeatureFlag
{
    std::int64_t id{0};
    std::string key;
    std::string description;
    bool enabled{false};
    std::int16_t rolloutPct{0};
    nlohmann::json targeting = nlohmann::json::object();
    std::string updatedAt;
};

/** Result of a single flag evaluation. */
struct FlagEvaluation
{
    std::string key;
    bool enabled{false};
    std::string reason;
};

/** Patch payload for the admin update endpoint. */
struct FlagPatch
{
    std::optional<std::string> description;
    std::optional<bool> enabled;
    std::optional<std::int16_t> rolloutPct;
    std::optional<nlohmann::json> targeting;
};

} // namespace services::flags
