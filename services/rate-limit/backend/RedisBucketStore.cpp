/**
 * @file RedisBucketStore.cpp
 * @brief RedisBucketStore::take — atomic single-token take.
 */

#include "rate-limit/backend/RedisBucketStore.h"
#include "rate-limit/backend/TokenBucket.h"
#include "infra/backend/RedisClient.h"

#include <chrono>
#include <string>
#include <vector>

namespace nextra::ratelimit
{

long long RedisBucketStore::nowMillis()
{
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch())
        .count();
}

BucketResult RedisBucketStore::take(
    const RateLimitKey& key,
    const BucketSpec& spec)
{
    // Fail open if the bucket is misconfigured.
    if (spec.capacity <= 0 || spec.refillPerSec <= 0.0) {
        return {true, spec.capacity, 0};
    }

    const std::string k = TokenBucket::redisKey(key);
    const std::string cap =
        std::to_string(spec.capacity);
    const std::string rate =
        std::to_string(spec.refillPerSec);
    const std::string now =
        std::to_string(nowMillis());
    // Keep bucket alive at least two refill windows so idle
    // tenants do not ping-pong between fresh and missing.
    const long long ttlSecs = static_cast<long long>(
        (spec.capacity / spec.refillPerSec) * 2.0) + 1;
    const std::string ttl = std::to_string(ttlSecs);

    const std::vector<std::string> keys{k};
    const std::vector<std::string> args{
        cap, rate, now, ttl};

    auto& redis = nextra::infra::RedisClient::instance();
    const std::string reply = redis.evalScript(
        std::string(TokenBucket::luaScript()),
        keys, args);
    return TokenBucket::parseReply(reply);
}

} // namespace nextra::ratelimit
