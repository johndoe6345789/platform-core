/**
 * @file test_audit_writer.cpp
 * @brief AuditWriter canonical event serialization.
 */

#include <gtest/gtest.h>
#include <string>

namespace
{

struct AuditEvent
{
    std::string actor;
    std::string action;
    std::string target;
};

/// @brief Canonical JSON (keys sorted, no whitespace).
std::string toCanonical(const AuditEvent& e)
{
    return std::string("{\"action\":\"") + e.action +
           "\",\"actor\":\"" + e.actor +
           "\",\"target\":\"" + e.target + "\"}";
}

} // namespace

class AuditWriterTest : public ::testing::Test
{
};

TEST_F(AuditWriterTest, CanonicalKeysSorted)
{
    AuditEvent e{"alice", "login", "session"};
    EXPECT_EQ(toCanonical(e),
              "{\"action\":\"login\","
              "\"actor\":\"alice\","
              "\"target\":\"session\"}");
}

TEST_F(AuditWriterTest, EmptyFieldsAllowed)
{
    AuditEvent e{"", "", ""};
    EXPECT_EQ(toCanonical(e),
              "{\"action\":\"\","
              "\"actor\":\"\","
              "\"target\":\"\"}");
}
