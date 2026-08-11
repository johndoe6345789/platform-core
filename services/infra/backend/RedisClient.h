#pragma once
/**
 * @file RedisClient.h
 * @brief Thin synchronous wrapper around hiredis.
 *
 * Choice: hiredis (official Redis C client) over cpp-redis.
 * Rationale: hiredis is a first-party, actively maintained,
 * trivially small dependency with an excellent Conan 2 package
 * (`hiredis/1.2.0`). cpp-redis has been effectively unmaintained
 * since 2020 and has no reliable Conan recipe. A thin RAII C++
 * facade gives us the ergonomics of cpp-redis without the
 * maintenance risk.
 *
 * A connection is lazily established on first use via
 * RedisClient::instance(). Host / port come from the
 * REDIS_HOST / REDIS_PORT environment variables, falling
 * back to the defaults in constants/infra.json.
 */

#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

struct redisContext;

namespace nextra::infra
{

/**
 * @class RedisClient
 * @brief Minimal synchronous Redis facade.
 */
class RedisClient
{
  public:
    /**
     * @brief Singleton accessor; lazy-connects on first use.
     * @return Reference to the process-wide instance.
     */
    static RedisClient& instance();

    /**
     * @brief GET a string key.
     * @param key Redis key.
     * @return Value if present, std::nullopt if missing.
     */
    std::optional<std::string> get(
        const std::string& key);

    /**
     * @brief SET a key with optional TTL (seconds, 0 = none).
     * @return True on success.
     */
    bool set(
        const std::string& key,
        const std::string& value,
        int ttlSeconds = 0);

    /**
     * @brief INCR a counter key.
     * @return New value after increment, -1 on error.
     */
    long long incr(const std::string& key);

    /**
     * @brief Run a Lua script via the Redis EVAL command.
     * @param script Lua source.
     * @param keys   KEYS[] list.
     * @param args   ARGV[] list.
     * @return Raw reply as a string, empty on error.
     */
    std::string evalScript(
        const std::string& script,
        const std::vector<std::string>& keys,
        const std::vector<std::string>& args);

    /** @brief True if the client has a live connection. */
    bool isConnected();

  private:
    RedisClient();
    ~RedisClient();
    RedisClient(const RedisClient&) = delete;
    RedisClient& operator=(const RedisClient&) = delete;

    /** @brief Establish or re-establish connection. */
    void connect();

    std::mutex mu_;
    redisContext* ctx_ = nullptr;
    std::string host_;
    std::uint16_t port_ = 6379;
};

} // namespace nextra::infra
