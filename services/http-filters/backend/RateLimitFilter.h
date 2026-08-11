#pragma once
/**
 * @file RateLimitFilter.h
 * @brief Simple in-memory per-IP rate limiter filter.
 *
 * Configurable via Drogon custom config key
 * "rate_limit_rpm" (requests per minute). Default: 60.
 */

#include <atomic>
#include <chrono>
#include <drogon/HttpFilter.h>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace filters
{

class RateLimitFilter : public drogon::HttpFilter<RateLimitFilter>
{
  public:
    RateLimitFilter() = default;

    /**
     * @brief Check per-IP request rate; reject with 429.
     * @param req  Incoming HTTP request.
     * @param cb   Callback to invoke on rate limit exceeded.
     * @param ccb  Chain callback to continue the pipeline.
     */
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& cb,
                  drogon::FilterChainCallback&& ccb) override;

  private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    struct Bucket {
        std::vector<TimePoint> timestamps;
        TimePoint lastSeen;
    };

    /// Sweep stale buckets every N requests.
    static constexpr int SWEEP_INTERVAL = 1000;

    /// Evict buckets idle longer than this duration.
    static constexpr std::chrono::minutes BUCKET_TTL{10};

    /// Hard cap on number of tracked IPs; oldest evicted when exceeded.
    static constexpr std::size_t MAX_BUCKETS = 100'000;

    std::mutex mutex_;
    std::unordered_map<std::string, Bucket> buckets_;
    std::atomic<int> requestCount_{0};

    /**
     * @brief Erase buckets not seen within BUCKET_TTL.
     * @param now  Current time snapshot.
     *
     * Must be called with @c mutex_ held.
     */
    void sweepStaleBuckets(TimePoint now);

    /**
     * @brief Read configured requests-per-minute limit.
     * @return Limit value from config or 60.
     */
    [[nodiscard]] static auto getLimit() -> int;
};

} // namespace filters
