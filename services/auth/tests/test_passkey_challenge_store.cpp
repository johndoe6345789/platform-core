/**
 * @file test_passkey_challenge_store.cpp
 * @brief ChallengeStore put/consume/sweep + replay.
 */

#include <gtest/gtest.h>
#include <chrono>
#include <string>
#include <unordered_map>

namespace
{

using Clock = std::chrono::steady_clock;

struct ChallengeStore
{
    struct Entry { Clock::time_point exp; };
    std::unordered_map<std::string, Entry> m;

    void put(const std::string& k, Clock::time_point e)
    {
        m[k] = {e};
    }
    bool consume(const std::string& k,
                 Clock::time_point now)
    {
        auto it = m.find(k);
        if (it == m.end()) return false;
        if (it->second.exp < now)
        {
            m.erase(it);
            return false;
        }
        m.erase(it);
        return true;
    }
    void sweep(Clock::time_point now)
    {
        for (auto it = m.begin(); it != m.end();)
        {
            if (it->second.exp < now)
                it = m.erase(it);
            else
                ++it;
        }
    }
};

} // namespace

class ChallengeStoreTest : public ::testing::Test
{
protected:
    ChallengeStore store;
    Clock::time_point now = Clock::now();
};

TEST_F(ChallengeStoreTest, PutThenConsumeOk)
{
    store.put("c1", now + std::chrono::seconds(60));
    EXPECT_TRUE(store.consume("c1", now));
}

TEST_F(ChallengeStoreTest, ReplayRejected)
{
    store.put("c1", now + std::chrono::seconds(60));
    EXPECT_TRUE(store.consume("c1", now));
    EXPECT_FALSE(store.consume("c1", now));
}

TEST_F(ChallengeStoreTest, ExpiredSweepRemoves)
{
    store.put("c1", now - std::chrono::seconds(1));
    store.sweep(now);
    EXPECT_FALSE(store.consume("c1", now));
}

TEST_F(ChallengeStoreTest, UnknownKeyRejected)
{
    EXPECT_FALSE(store.consume("none", now));
}
