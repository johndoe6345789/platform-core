#pragma once
/**
 * @file TokenBucket.h
 * @brief Token-bucket algorithm expressed as a Lua script.
 *
 * The bucket state lives in Redis under a hash with fields
 * `tokens` (current balance) and `ts` (last refill at unix
 * millis). The Lua script runs server-side and atomically
 * refills + takes a single token, returning:
 *
 *   { allowed, remaining, retryAfterSeconds }
 *
 * See RedisBucketStore for the C++ caller that wraps this.
 */

#include "rate-limit/backend/RateLimitTypes.h"

#include <string>
#include <string_view>

namespace nextra::ratelimit
{

/**
 * @class TokenBucket
 * @brief Static helpers that describe the token-bucket Lua
 *        contract used by RedisBucketStore.
 */
class TokenBucket
{
  public:
    /**
     * @brief Return the Lua script source.
     *
     * KEYS[1] = hash key, ARGV[1] = capacity,
     * ARGV[2] = refillPerSec (float),
     * ARGV[3] = nowMillis, ARGV[4] = ttlSeconds.
     *
     * @return Lua source as a static string view.
     */
    static std::string_view luaScript() noexcept;

    /**
     * @brief Build the composite Redis hash key.
     * @param key Logical rate-limit key.
     * @return Namespaced Redis key string.
     */
    static std::string redisKey(const RateLimitKey& key);

    /**
     * @brief Parse the three-element Lua reply payload.
     *
     * The RedisClient::evalScript() wrapper currently returns
     * either a string or an integer — for our 3-element array
     * reply we encode as "allowed|remaining|retry" inside the
     * Lua script itself, then parse back here.
     *
     * @param raw Raw payload string from Redis.
     * @return Decoded BucketResult. Fails open (allowed=true)
     *         when parsing fails so Redis outages do not down
     *         the API.
     */
    static BucketResult parseReply(const std::string& raw);
};

} // namespace nextra::ratelimit
