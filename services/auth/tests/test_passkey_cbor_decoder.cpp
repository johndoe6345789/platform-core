/**
 * @file test_passkey_cbor_decoder.cpp
 * @brief CBOR decoding happy path and truncation.
 *        Mirrors the minimal subset used by CoseKey.
 */

#include <gtest/gtest.h>
#include <cstdint>
#include <optional>
#include <vector>

namespace
{

/// @brief Reads a CBOR unsigned int (major type 0).
/// @return value or nullopt on truncation.
std::optional<uint64_t> readUint(
    const std::vector<uint8_t>& b, size_t& i)
{
    if (i >= b.size()) return std::nullopt;
    uint8_t ib = b[i++];
    uint8_t mt = ib >> 5;
    uint8_t ai = ib & 0x1f;
    if (mt != 0) return std::nullopt;
    if (ai < 24) return ai;
    if (ai == 24)
    {
        if (i >= b.size()) return std::nullopt;
        return b[i++];
    }
    return std::nullopt;
}

} // namespace

class CborDecoderTest : public ::testing::Test
{
};

TEST_F(CborDecoderTest, SmallUintInline)
{
    std::vector<uint8_t> b = {0x07};
    size_t i = 0;
    auto v = readUint(b, i);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, 7u);
}

TEST_F(CborDecoderTest, OneByteUint)
{
    std::vector<uint8_t> b = {0x18, 0x2a};
    size_t i = 0;
    auto v = readUint(b, i);
    ASSERT_TRUE(v.has_value());
    EXPECT_EQ(*v, 42u);
}

TEST_F(CborDecoderTest, TruncatedReturnsNullopt)
{
    std::vector<uint8_t> b = {0x18};
    size_t i = 0;
    EXPECT_FALSE(readUint(b, i).has_value());
}

TEST_F(CborDecoderTest, WrongMajorTypeRejected)
{
    std::vector<uint8_t> b = {0x60};
    size_t i = 0;
    EXPECT_FALSE(readUint(b, i).has_value());
}
