/**
 * @file redis_client_connect.cpp
 * @brief RedisClient construction + connection lifecycle.
 */

#include "infra/backend/RedisClient.h"

#include <hiredis/hiredis.h>
#include <spdlog/spdlog.h>

#include <cstdlib>
#include <format>

namespace nextra::infra
{

namespace
{
constexpr const char* kDefaultHost = "redis";
constexpr std::uint16_t kDefaultPort = 6379;

std::string envOr(const char* k, const char* def)
{
    const char* v = std::getenv(k);
    return v ? std::string(v) : std::string(def);
}
} // namespace

RedisClient& RedisClient::instance()
{
    static RedisClient inst;
    return inst;
}

RedisClient::RedisClient()
    : host_(envOr("REDIS_HOST", kDefaultHost))
{
    const char* p = std::getenv("REDIS_PORT");
    port_ = static_cast<std::uint16_t>(
        p ? std::atoi(p) : kDefaultPort);
    connect();
}

RedisClient::~RedisClient()
{
    if (ctx_ != nullptr) {
        redisFree(ctx_);
        ctx_ = nullptr;
    }
}

void RedisClient::connect()
{
    if (ctx_ != nullptr) {
        redisFree(ctx_);
        ctx_ = nullptr;
    }
    struct timeval tv {
        2, 0
    };
    ctx_ = redisConnectWithTimeout(
        host_.c_str(), port_, tv);
    if (ctx_ == nullptr || ctx_->err != 0) {
        spdlog::warn(
            "RedisClient connect {}:{} failed: {}",
            host_, port_,
            ctx_ ? ctx_->errstr : "alloc");
        if (ctx_ != nullptr) {
            redisFree(ctx_);
            ctx_ = nullptr;
        }
        return;
    }
    spdlog::info(
        "RedisClient -> {}:{}", host_, port_);
}

bool RedisClient::isConnected()
{
    std::lock_guard<std::mutex> lk(mu_);
    if (ctx_ == nullptr) {
        connect();
    }
    if (ctx_ == nullptr) {
        return false;
    }
    auto* r = static_cast<redisReply*>(
        redisCommand(ctx_, "PING"));
    if (r == nullptr) {
        return false;
    }
    bool ok = (r->type == REDIS_REPLY_STATUS
        && std::string(r->str) == "PONG");
    freeReplyObject(r);
    return ok;
}

} // namespace nextra::infra
