/**
 * @file TokenBucket.cpp
 * @brief TokenBucket Lua script + reply parser.
 */

#include "rate-limit/backend/TokenBucket.h"

#include <charconv>
#include <cstdlib>
#include <string>

namespace nextra::ratelimit
{

namespace
{

/** @brief Lua script: atomic refill + single-token take. */
constexpr const char* kLua = R"LUA(
local k = KEYS[1]
local cap = tonumber(ARGV[1])
local rate = tonumber(ARGV[2])
local now = tonumber(ARGV[3])
local ttl = tonumber(ARGV[4])
local h = redis.call('HMGET', k, 'tokens', 'ts')
local tokens = tonumber(h[1]) or cap
local ts = tonumber(h[2]) or now
local delta = math.max(0, now - ts) / 1000.0
tokens = math.min(cap, tokens + delta * rate)
local allowed = 0
local retry = 0
if tokens >= 1 then
  tokens = tokens - 1
  allowed = 1
else
  retry = math.ceil((1 - tokens) / rate)
end
redis.call('HMSET', k, 'tokens', tokens, 'ts', now)
redis.call('EXPIRE', k, ttl)
return allowed .. '|' .. math.floor(tokens) .. '|' .. retry
)LUA";

} // namespace

std::string_view TokenBucket::luaScript() noexcept
{
    return kLua;
}

std::string TokenBucket::redisKey(const RateLimitKey& key)
{
    return "rl:" + key.tenantId + ":" + key.routeGroup;
}

BucketResult TokenBucket::parseReply(const std::string& raw)
{
    BucketResult r{true, 0, 0};
    if (raw.empty()) {
        return r; // fail open
    }
    const auto p1 = raw.find('|');
    if (p1 == std::string::npos) {
        return r;
    }
    const auto p2 = raw.find('|', p1 + 1);
    if (p2 == std::string::npos) {
        return r;
    }
    r.allowed = (raw[0] == '1');
    r.remaining = std::atoll(
        raw.substr(p1 + 1, p2 - p1 - 1).c_str());
    r.retryAfter = std::atoll(
        raw.substr(p2 + 1).c_str());
    return r;
}

} // namespace nextra::ratelimit
