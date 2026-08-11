/**
 * @file test_passkey_cose_key_parser.cpp
 * @brief ES256 vs RS256 alg detection for COSE keys.
 *        Mirrors services/auth/passkeys/CoseKey.cpp alg
 *        field extraction.
 */

#include <gtest/gtest.h>

namespace
{

enum class CoseAlg
{
    Unknown,
    Es256,
    Rs256
};

/// @brief Maps COSE alg integer to internal enum.
CoseAlg algFromInt(int v)
{
    if (v == -7) return CoseAlg::Es256;
    if (v == -257) return CoseAlg::Rs256;
    return CoseAlg::Unknown;
}

} // namespace

class CoseKeyParserTest : public ::testing::Test
{
};

TEST_F(CoseKeyParserTest, Es256Mapped)
{
    EXPECT_EQ(algFromInt(-7), CoseAlg::Es256);
}

TEST_F(CoseKeyParserTest, Rs256Mapped)
{
    EXPECT_EQ(algFromInt(-257), CoseAlg::Rs256);
}

TEST_F(CoseKeyParserTest, UnknownAlgRejected)
{
    EXPECT_EQ(algFromInt(0), CoseAlg::Unknown);
    EXPECT_EQ(algFromInt(-8), CoseAlg::Unknown);
}
