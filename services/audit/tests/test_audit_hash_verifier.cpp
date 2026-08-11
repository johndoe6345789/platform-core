/**
 * @file test_audit_hash_verifier.cpp
 * @brief Detects tampering in a recorded chain.
 */

#include <gtest/gtest.h>
#include <functional>
#include <string>
#include <vector>

namespace
{

struct Row
{
    std::string body;
    std::string hash;
};

std::string step(const std::string& prev,
                 const std::string& body)
{
    std::hash<std::string> h;
    return std::to_string(h(prev + "|" + body));
}

bool verify(const std::vector<Row>& rows)
{
    std::string prev;
    for (const auto& r : rows)
    {
        if (step(prev, r.body) != r.hash) return false;
        prev = r.hash;
    }
    return true;
}

std::vector<Row> build(const std::vector<std::string>& bs)
{
    std::vector<Row> out;
    std::string prev;
    for (const auto& b : bs)
    {
        auto h = step(prev, b);
        out.push_back({b, h});
        prev = h;
    }
    return out;
}

} // namespace

class HashVerifierTest : public ::testing::Test
{
};

TEST_F(HashVerifierTest, CleanChainVerifies)
{
    auto rows = build({"a", "b", "c"});
    EXPECT_TRUE(verify(rows));
}

TEST_F(HashVerifierTest, TamperedBodyDetected)
{
    auto rows = build({"a", "b", "c"});
    rows[1].body = "B";
    EXPECT_FALSE(verify(rows));
}

TEST_F(HashVerifierTest, TamperedHashDetected)
{
    auto rows = build({"a", "b", "c"});
    rows[2].hash = "nope";
    EXPECT_FALSE(verify(rows));
}
