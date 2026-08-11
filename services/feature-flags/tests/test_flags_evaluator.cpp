/**
 * @file test_flags_evaluator.cpp
 * @brief FlagEvaluator: percentage rollout, user target,
 *        global kill switch.
 */

#include <gtest/gtest.h>
#include <functional>
#include <string>
#include <unordered_set>

namespace
{

struct Flag
{
    bool killed = false;
    int pct = 0;
    std::unordered_set<std::string> targeted;
};

bool isEnabled(const Flag& f, const std::string& userId)
{
    if (f.killed) return false;
    if (f.targeted.count(userId)) return true;
    auto h = std::hash<std::string>{}(userId);
    return static_cast<int>(h % 100) < f.pct;
}

} // namespace

class FlagEvaluatorTest : public ::testing::Test
{
};

TEST_F(FlagEvaluatorTest, KillSwitchWins)
{
    Flag f;
    f.killed = true;
    f.pct = 100;
    f.targeted = {"u1"};
    EXPECT_FALSE(isEnabled(f, "u1"));
}

TEST_F(FlagEvaluatorTest, TargetedUserOn)
{
    Flag f;
    f.targeted = {"u1"};
    EXPECT_TRUE(isEnabled(f, "u1"));
}

TEST_F(FlagEvaluatorTest, ZeroPctNoMatch)
{
    Flag f;
    EXPECT_FALSE(isEnabled(f, "u1"));
}

TEST_F(FlagEvaluatorTest, HundredPctAlwaysOn)
{
    Flag f;
    f.pct = 100;
    EXPECT_TRUE(isEnabled(f, "u1"));
    EXPECT_TRUE(isEnabled(f, "u2"));
}

TEST_F(FlagEvaluatorTest, DeterministicForUser)
{
    Flag f;
    f.pct = 50;
    EXPECT_EQ(isEnabled(f, "u1"), isEnabled(f, "u1"));
}
