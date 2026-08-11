#pragma once
/**
 * @file RateLimitTypes.h
 * @brief Shared types for the Redis token-bucket rate limiter.
 *
 * The rate limiter keys by (tenant_id, route_group). Each key
 * owns a Redis hash with two fields (tokens, ts) driven by a
 * single Lua script for atomic take+refill semantics.
 */

#include <cstdint>
#include <string>

namespace nextra::ratelimit
{

/**
 * @struct BucketSpec
 * @brief Describes the token bucket shape for a route group.
 *
 * `capacity` is the maximum burst; `refillPerSec` is how many
 * tokens are added to the bucket per second. A request costs
 * one token.
 */
struct BucketSpec
{
    /** @brief Maximum bucket size (burst). */
    std::int64_t capacity{0};

    /** @brief Refill rate in tokens per second. */
    double refillPerSec{0.0};
};

/**
 * @struct BucketResult
 * @brief Outcome of a single take() attempt.
 */
struct BucketResult
{
    /** @brief True if the request was permitted. */
    bool allowed{false};

    /** @brief Tokens remaining after the take (>= 0). */
    std::int64_t remaining{0};

    /** @brief Seconds until the next token is available. */
    std::int64_t retryAfter{0};
};

/**
 * @struct RateLimitKey
 * @brief Composite bucket identity: tenant plus route group.
 */
struct RateLimitKey
{
    /** @brief Tenant identifier (falls back to client IP). */
    std::string tenantId;

    /** @brief Logical route group (admin, auth, public, ...). */
    std::string routeGroup;
};

} // namespace nextra::ratelimit
