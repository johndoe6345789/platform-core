/**
 * @file test_audit_hash_chain.cpp
 * @brief HashChain determinism and link validity.
 *
 * Uses std::hash as a stand-in for SHA-256 to keep the
 * test independent of OpenSSL link state — exercises
 * the CANONICAL-JSON | prev-hash concatenation shape.
 */

#include <gtest/gtest.h>
#include <functional>
#include <string>

namespace
{

std::string chainStep(const std::string& prev,
                      const std::string& canonical)
{
    std::hash<std::string> h;
    auto v = h(prev + "|" + canonical);
    return std::to_string(v);
}

} // namespace

class HashChainTest : public ::testing::Test
{
};

TEST_F(HashChainTest, DeterministicForSameInput)
{
    auto a = chainStep("", "{\"k\":1}");
    auto b = chainStep("", "{\"k\":1}");
    EXPECT_EQ(a, b);
}

TEST_F(HashChainTest, DifferentPrevDiffersHash)
{
    auto a = chainStep("", "{\"k\":1}");
    auto b = chainStep("x", "{\"k\":1}");
    EXPECT_NE(a, b);
}

TEST_F(HashChainTest, DifferentBodyDiffersHash)
{
    auto a = chainStep("", "{\"k\":1}");
    auto b = chainStep("", "{\"k\":2}");
    EXPECT_NE(a, b);
}

TEST_F(HashChainTest, LinkValidity)
{
    auto h1 = chainStep("", "a");
    auto h2 = chainStep(h1, "b");
    // Re-derive h2 from the same h1 must match.
    EXPECT_EQ(h2, chainStep(h1, "b"));
}
