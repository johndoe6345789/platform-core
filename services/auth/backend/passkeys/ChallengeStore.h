#pragma once
/**
 * @file ChallengeStore.h
 * @brief Thread-safe in-memory TTL store for WebAuthn challenges.
 *
 * Challenges live for a configurable TTL (default 300s) and are
 * removed on first successful consume() call or on expiry sweep.
 * A Redis-backed variant can slot in behind the same API later.
 */

#include "PasskeyTypes.h"

#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace services::auth::passkeys
{

/** @brief Singleton TTL map keyed by base64url challenge. */
class ChallengeStore
{
  public:
    /** @brief Process-wide singleton access. */
    static ChallengeStore& instance();

    /** @brief Insert a new pending challenge. */
    void put(const std::string& key,
             const PendingChallenge& value);

    /** @brief Consume a challenge (removes if present). */
    std::optional<PendingChallenge> consume(
        const std::string& key);

    /** @brief Drop all expired entries. */
    void sweep(int ttlSeconds);

  private:
    ChallengeStore() = default;
    std::mutex mu_;
    std::unordered_map<std::string, PendingChallenge> map_;
};

} // namespace services::auth::passkeys
