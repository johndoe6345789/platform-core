/**
 * @file test_totp_base32.cpp
 * @brief RFC 4648 Base32 decoder test vectors.
 */

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace
{

/// @brief RFC 4648 Base32 decode (no padding required).
std::optional<std::vector<uint8_t>> b32decode(
    const std::string& in)
{
    static const std::string A =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
    std::vector<uint8_t> out;
    uint32_t buf = 0;
    int bits = 0;
    for (char c : in)
    {
        if (c == '=' || c == ' ') continue;
        auto p = A.find(c);
        if (p == std::string::npos) return std::nullopt;
        buf = (buf << 5) | static_cast<uint32_t>(p);
        bits += 5;
        if (bits >= 8)
        {
            bits -= 8;
            out.push_back(
                static_cast<uint8_t>((buf >> bits) & 0xff));
        }
    }
    return out;
}

} // namespace

class Base32Test : public ::testing::Test
{
};

TEST_F(Base32Test, RfcVectorFoo)
{
    auto r = b32decode("MZXW6===");
    ASSERT_TRUE(r.has_value());
    std::string s(r->begin(), r->end());
    EXPECT_EQ(s, "foo");
}

TEST_F(Base32Test, RfcVectorFooba)
{
    auto r = b32decode("MZXW6YTB");
    ASSERT_TRUE(r.has_value());
    std::string s(r->begin(), r->end());
    EXPECT_EQ(s, "fooba");
}

TEST_F(Base32Test, EmptyIsEmpty)
{
    auto r = b32decode("");
    ASSERT_TRUE(r.has_value());
    EXPECT_TRUE(r->empty());
}

TEST_F(Base32Test, InvalidCharRejected)
{
    EXPECT_FALSE(b32decode("MZXW6!TB").has_value());
}
