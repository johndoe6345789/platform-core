/**
 * @file test_ratelimit_token_bucket.cpp
 * @brief TokenBucket consume/refill over simulated time.
 */

#include <gtest/gtest.h>
#include <algorithm>

namespace
{

struct TokenBucket
{
    double capacity;
    double refillPerSec;
    double tokens;
    double lastTs;

    bool consume(double ts, double n)
    {
        double dt = ts - lastTs;
        if (dt > 0)
            tokens = std::min(
                capacity, tokens + dt * refillPerSec);
        lastTs = ts;
        if (tokens < n) return false;
        tokens -= n;
        return true;
    }
};

} // namespace

class TokenBucketTest : public ::testing::Test
{
protected:
    TokenBucket b{10.0, 1.0, 10.0, 0.0};
};

TEST_F(TokenBucketTest, ConsumesAvailable)
{
    EXPECT_TRUE(b.consume(0.0, 5.0));
    EXPECT_DOUBLE_EQ(b.tokens, 5.0);
}

TEST_F(TokenBucketTest, DeniesWhenEmpty)
{
    EXPECT_TRUE(b.consume(0.0, 10.0));
    EXPECT_FALSE(b.consume(0.0, 1.0));
}

TEST_F(TokenBucketTest, RefillsOverTime)
{
    b.consume(0.0, 10.0);
    EXPECT_TRUE(b.consume(3.0, 3.0));
}

TEST_F(TokenBucketTest, CapRespected)
{
    b.consume(0.0, 5.0);
    b.consume(1000.0, 0.0);
    EXPECT_DOUBLE_EQ(b.tokens, 10.0);
}
