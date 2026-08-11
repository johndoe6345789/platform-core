#pragma once
/**
 * @file RedisBucketStore.h
 * @brief Redis-backed bucket store for the rate limiter.
 *
 * Thin wrapper that composes TokenBucket (the algorithm) with
 * RedisClient::evalScript (the transport). Fails open on any
 * Redis error so a Redis outage does not turn into a blanket
 * denial of service.
 */

#include "rate-limit/backend/RateLimitTypes.h"

#include <string>

namespace nextra::ratelimit
{

/**
 * @class RedisBucketStore
 * @brief Atomic take-one operation against a Redis bucket.
 */
class RedisBucketStore
{
  public:
    /**
     * @brief Attempt to consume one token from the bucket.
     * @param key  Composite (tenant, route_group) identity.
     * @param spec Bucket shape (capacity + refill rate).
     * @return BucketResult describing the outcome.
     */
    static BucketResult take(
        const RateLimitKey& key,
        const BucketSpec& spec);

  private:
    /**
     * @brief Return wall clock milliseconds since the epoch.
     * @return Current time in milliseconds.
     */
    static long long nowMillis();
};

} // namespace nextra::ratelimit
