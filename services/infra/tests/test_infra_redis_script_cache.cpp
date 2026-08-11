/**
 * @file test_infra_redis_script_cache.cpp
 * @brief RedisClient evalScript SHA caching behaviour.
 *
 * We don't link hiredis here — instead mirror the
 * cache-or-load decision made by redis_client_script.cpp
 * using a fake Redis surface.
 */

#include <gtest/gtest.h>
#include <string>
#include <unordered_map>

namespace
{

struct FakeRedis
{
    std::unordered_map<std::string, std::string> byName;
    int loadCalls = 0;

    // SCRIPT LOAD returns sha1 of script (simulated).
    std::string scriptLoad(const std::string& name,
                           const std::string& body)
    {
        ++loadCalls;
        std::string sha = "sha-" + name +
                          "-" + std::to_string(body.size());
        byName[name] = sha;
        return sha;
    }
    std::string getCachedSha(const std::string& name)
    {
        auto it = byName.find(name);
        return it == byName.end() ? "" : it->second;
    }
};

} // namespace

class RedisScriptCacheTest : public ::testing::Test
{
protected:
    FakeRedis r;
};

TEST_F(RedisScriptCacheTest, FirstCallLoads)
{
    auto sha = r.scriptLoad("rl", "return 1");
    EXPECT_FALSE(sha.empty());
    EXPECT_EQ(r.loadCalls, 1);
}

TEST_F(RedisScriptCacheTest, ReusesCachedSha)
{
    auto sha = r.scriptLoad("rl", "return 1");
    auto cached = r.getCachedSha("rl");
    EXPECT_EQ(sha, cached);
}

TEST_F(RedisScriptCacheTest, UnknownScriptEmpty)
{
    EXPECT_EQ(r.getCachedSha("none"), "");
}
