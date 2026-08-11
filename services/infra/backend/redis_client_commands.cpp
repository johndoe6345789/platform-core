/**
 * @file redis_client_commands.cpp
 * @brief RedisClient basic commands (GET, SET, INCR).
 *
 * The scripted EVAL command lives in
 * redis_client_script.cpp to keep each file ≤100 lines.
 */

#include "infra/backend/RedisClient.h"
#include "infra/backend/redis_reply_guard.h"

#include <hiredis/hiredis.h>

namespace nextra::infra
{

std::optional<std::string> RedisClient::get(
    const std::string& key)
{
    std::lock_guard<std::mutex> lk(mu_);
    if (ctx_ == nullptr) {
        return std::nullopt;
    }
    ReplyGuard g(redisCommand(
        ctx_, "GET %s", key.c_str()));
    if (g.r == nullptr
        || g.r->type == REDIS_REPLY_NIL) {
        return std::nullopt;
    }
    if (g.r->type != REDIS_REPLY_STRING) {
        return std::nullopt;
    }
    return std::string(g.r->str, g.r->len);
}

bool RedisClient::set(
    const std::string& key,
    const std::string& value,
    int ttlSeconds)
{
    std::lock_guard<std::mutex> lk(mu_);
    if (ctx_ == nullptr) {
        return false;
    }
    ReplyGuard g(
        ttlSeconds > 0
            ? redisCommand(
                  ctx_, "SET %s %s EX %d",
                  key.c_str(), value.c_str(),
                  ttlSeconds)
            : redisCommand(
                  ctx_, "SET %s %s",
                  key.c_str(), value.c_str()));
    return g.r != nullptr
        && g.r->type == REDIS_REPLY_STATUS;
}

long long RedisClient::incr(const std::string& key)
{
    std::lock_guard<std::mutex> lk(mu_);
    if (ctx_ == nullptr) {
        return -1;
    }
    ReplyGuard g(redisCommand(
        ctx_, "INCR %s", key.c_str()));
    if (g.r == nullptr
        || g.r->type != REDIS_REPLY_INTEGER) {
        return -1;
    }
    return g.r->integer;
}

} // namespace nextra::infra
