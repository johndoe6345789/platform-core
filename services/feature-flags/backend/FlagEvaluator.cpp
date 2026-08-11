/**
 * @file FlagEvaluator.cpp
 * @brief Deterministic hash-bucket rollout implementation.
 */

#include "FlagEvaluator.h"

namespace services::flags
{

std::uint32_t
FlagEvaluator::fnv1a(const std::string& input) noexcept
{
    constexpr std::uint32_t offset = 0x811C9DC5u;
    constexpr std::uint32_t prime = 0x01000193u;
    std::uint32_t hash = offset;
    for (unsigned char c : input) {
        hash ^= c;
        hash *= prime;
    }
    return hash;
}

std::uint32_t
FlagEvaluator::bucket(const std::string& key,
                      const std::string& userId) noexcept
{
    return fnv1a(key + ":" + userId) % 100u;
}

FlagEvaluation
FlagEvaluator::evaluate(const FeatureFlag& flag,
                        const std::string& userId)
{
    FlagEvaluation out{flag.key, false, ""};

    if (!flag.enabled) {
        out.reason = "disabled";
        return out;
    }

    // Targeting: explicit user allow/deny lists.
    if (flag.targeting.is_object()) {
        auto denyIt = flag.targeting.find("deny");
        if (denyIt != flag.targeting.end()
            && denyIt->is_array()) {
            for (const auto& u : *denyIt) {
                if (u.is_string()
                    && u.get<std::string>() == userId) {
                    out.reason = "deny_list";
                    return out;
                }
            }
        }
        auto allowIt = flag.targeting.find("allow");
        if (allowIt != flag.targeting.end()
            && allowIt->is_array()) {
            for (const auto& u : *allowIt) {
                if (u.is_string()
                    && u.get<std::string>() == userId) {
                    out.enabled = true;
                    out.reason = "allow_list";
                    return out;
                }
            }
        }
    }

    if (flag.rolloutPct <= 0) {
        out.reason = "rollout_zero";
        return out;
    }
    if (flag.rolloutPct >= 100) {
        out.enabled = true;
        out.reason = "rollout_full";
        return out;
    }

    const auto b = bucket(flag.key, userId);
    if (static_cast<int>(b)
        < static_cast<int>(flag.rolloutPct)) {
        out.enabled = true;
        out.reason = "rollout_bucket";
    } else {
        out.reason = "rollout_miss";
    }
    return out;
}

} // namespace services::flags
