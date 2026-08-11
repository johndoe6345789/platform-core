/**
 * @file RateLimitFilter.cpp
 * @brief In-memory sliding-window rate limiter.
 */

#include "RateLimitFilter.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "rate_limit_sweep.h"

#include <algorithm>
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace filters
{

auto RateLimitFilter::getLimit() -> int
{
    auto& cfg = drogon::app().getCustomConfig();
    if (cfg.isMember("rate_limit_rpm")
        && cfg["rate_limit_rpm"].isInt()) {
        return cfg["rate_limit_rpm"].asInt();
    }
    return 60;
}

void RateLimitFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& cb,
    drogon::FilterChainCallback&& ccb)
{
    const auto ip = req->peerAddr().toIp();
    const auto now = Clock::now();
    const auto limit = getLimit();
    const auto window = std::chrono::minutes{1};

    std::lock_guard lock{mutex_};

    auto& bucket = buckets_[ip];
    bucket.lastSeen = now;

    std::erase_if(bucket.timestamps,
                  [&](auto& ts) {
                      return (now - ts) > window;
                  });

    if (static_cast<int>(
            bucket.timestamps.size()) >= limit) {
        auto oldest = bucket.timestamps.front();
        auto retryAt = oldest + window;
        auto retrySec =
            std::chrono::duration_cast<
                std::chrono::seconds>(retryAt - now)
                .count();

        auto resp = ::utils::jsonError(
            drogon::k429TooManyRequests,
            "Rate limit exceeded");
        resp->addHeader(
            "Retry-After",
            std::to_string(std::max(1L, retrySec)));
        cb(resp);
        return;
    }

    bucket.timestamps.push_back(now);

    if (requestCount_.fetch_add(
            1, std::memory_order_relaxed)
        % SWEEP_INTERVAL == 0) {
        sweepStaleBuckets(now);
    }

    ccb();
}

void RateLimitFilter::sweepStaleBuckets(TimePoint now)
{
    sweepBuckets(
        buckets_, now, BUCKET_TTL, MAX_BUCKETS);
}

} // namespace filters
