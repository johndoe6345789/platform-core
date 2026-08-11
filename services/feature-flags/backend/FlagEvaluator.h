#pragma once
/**
 * @file FlagEvaluator.h
 * @brief Deterministic flag evaluation (hash-bucket rollout).
 */

#include "FlagTypes.h"

#include <cstdint>
#include <string>

namespace services::flags
{

/**
 * @brief Stateless evaluator for a FeatureFlag + user id.
 *
 * Evaluation is deterministic: FNV-1a hash of the composed
 * "<key>:<user_id>" string is taken modulo 100 and compared
 * against rollout_pct. The same (flag, user) pair always
 * yields the same answer as long as rollout_pct is stable.
 */
class FlagEvaluator
{
  public:
    /** @brief Evaluate flag for user; empty user → anon. */
    static FlagEvaluation
    evaluate(const FeatureFlag& flag,
             const std::string& userId);

    /** @brief FNV-1a 32-bit hash over an arbitrary string. */
    static std::uint32_t
    fnv1a(const std::string& input) noexcept;

    /** @brief Bucket (0..99) for a (key, userId) pair. */
    static std::uint32_t
    bucket(const std::string& key,
           const std::string& userId) noexcept;
};

} // namespace services::flags
