#pragma once
/**
 * @file rate_limit_sweep.h
 * @brief Stale-bucket eviction logic for
 *        RateLimitFilter.
 */

#include <algorithm>
#include <chrono>
#include <spdlog/spdlog.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace filters
{

/**
 * @brief Evict idle and excess buckets.
 *
 * Removes buckets older than @p ttl, then if the
 * count still exceeds @p maxBuckets, evicts the
 * oldest entries.
 *
 * @param buckets     The bucket map to sweep.
 * @param now         Current time snapshot.
 * @param ttl         Max idle duration.
 * @param maxBuckets  Hard cap on tracked IPs.
 */
template <typename Bucket, typename TimePoint>
void sweepBuckets(
    std::unordered_map<std::string, Bucket>& buckets,
    TimePoint now,
    std::chrono::minutes ttl,
    std::size_t maxBuckets)
{
    std::erase_if(buckets, [&](const auto& kv) {
        return (now - kv.second.lastSeen) > ttl;
    });

    if (buckets.size() <= maxBuckets) {
        return;
    }

    spdlog::warn(
        "RateLimitFilter: bucket count {} exceeds "
        "MAX_BUCKETS {}; evicting oldest entries.",
        buckets.size(), maxBuckets);

    using Iter = typename std::unordered_map<
        std::string, Bucket>::iterator;
    std::vector<Iter> order;
    order.reserve(buckets.size());
    for (auto it = buckets.begin();
         it != buckets.end(); ++it) {
        order.push_back(it);
    }
    std::sort(
        order.begin(), order.end(),
        [](const auto& a, const auto& b) {
            return a->second.lastSeen
                   < b->second.lastSeen;
        });

    const auto excess =
        buckets.size() - maxBuckets;
    for (std::size_t i = 0; i < excess; ++i) {
        buckets.erase(order[i]);
    }
}

} // namespace filters
