/**
 * @file redis_client_script.cpp
 * @brief RedisClient::evalScript — Lua script execution.
 *
 * Split from redis_client_commands.cpp to stay under the
 * project's 100-LOC-per-file cap.
 */

#include "infra/backend/RedisClient.h"
#include "infra/backend/redis_reply_guard.h"

#include <hiredis/hiredis.h>

#include <cstddef>
#include <string>
#include <vector>

namespace nextra::infra
{

std::string RedisClient::evalScript(
    const std::string& script,
    const std::vector<std::string>& keys,
    const std::vector<std::string>& args)
{
    std::lock_guard<std::mutex> lk(mu_);
    if (ctx_ == nullptr) {
        return {};
    }
    std::vector<const char*> argv;
    std::vector<std::size_t> argvLen;
    argv.reserve(3 + keys.size() + args.size());
    argvLen.reserve(argv.capacity());

    auto push = [&](const std::string& s) {
        argv.push_back(s.data());
        argvLen.push_back(s.size());
    };

    const std::string cmd = "EVAL";
    const std::string numK =
        std::to_string(keys.size());
    push(cmd);
    push(script);
    push(numK);
    for (const auto& k : keys) {
        push(k);
    }
    for (const auto& a : args) {
        push(a);
    }

    ReplyGuard g(redisCommandArgv(
        ctx_, static_cast<int>(argv.size()),
        argv.data(), argvLen.data()));
    if (g.r == nullptr) {
        return {};
    }
    if (g.r->type == REDIS_REPLY_STRING) {
        return std::string(g.r->str, g.r->len);
    }
    if (g.r->type == REDIS_REPLY_INTEGER) {
        return std::to_string(g.r->integer);
    }
    return {};
}

} // namespace nextra::infra
