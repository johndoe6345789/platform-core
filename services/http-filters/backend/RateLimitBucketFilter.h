#pragma once
/**
 * @file RateLimitBucketFilter.h
 * @brief Drogon filter — Redis token-bucket rate limit.
 *
 * Sits alongside the legacy in-memory `filters::RateLimitFilter`
 * from v1 and is the new default for any controller that wants
 * distributed, tenant-aware rate limiting. Wire via:
 *
 *     "Filters": ["nextra::filters::RateLimitFilter"]
 *
 * Groups + limits come from `constants/rate-limit.json`.
 */

#include "rate-limit/backend/RateLimitTypes.h"

#include <drogon/HttpFilter.h>
#include <string>

namespace nextra::filters
{

/**
 * @class RateLimitFilter
 * @brief HttpFilter that classifies the request, keys by
 *        tenant + route group, and consults a Redis bucket.
 */
class RateLimitFilter
    : public drogon::HttpFilter<RateLimitFilter>
{
  public:
    RateLimitFilter() = default;

    /**
     * @brief Check the bucket and either continue or 429.
     * @param req Incoming Drogon request.
     * @param cb  Short-circuit response callback.
     * @param ccb Continue-chain callback.
     */
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& cb,
        drogon::FilterChainCallback&& ccb) override;

  private:
    /** @brief Classify request path → route group name. */
    [[nodiscard]] static std::string classifyGroup(
        const std::string& path);

    /** @brief Resolve tenant id (header → cookie → IP). */
    [[nodiscard]] static std::string tenantOf(
        const drogon::HttpRequestPtr& req);

    /** @brief Look up the bucket spec for a group. */
    [[nodiscard]] static nextra::ratelimit::BucketSpec
    specFor(const std::string& group);
};

} // namespace nextra::filters
