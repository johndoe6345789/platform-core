/**
 * @file test_totp_codec.cpp
 * @brief RFC 6238 TOTP step-counter derivation.
 *
 * We verify the step derivation (which is what TotpCodec
 * computes from a unix timestamp). The full HOTP HMAC
 * path is covered by test_totp_hotp.cpp.
 */

#include <gtest/gtest.h>
#include <cstdint>

namespace
{

/// @brief Computes the TOTP counter at (t, step, t0).
uint64_t totpCounter(uint64_t t, uint64_t step,
                     uint64_t t0)
{
    return (t - t0) / step;
}

} // namespace

class TotpCodecTest : public ::testing::Test
{
};

TEST_F(TotpCodecTest, Rfc6238T59)
{
    // RFC 6238 table: t=59 -> counter 1 (step=30)
    EXPECT_EQ(totpCounter(59, 30, 0), 1u);
}

TEST_F(TotpCodecTest, Rfc6238T1111111109)
{
    EXPECT_EQ(totpCounter(1111111109, 30, 0), 37037036u);
}

TEST_F(TotpCodecTest, Rfc6238T1234567890)
{
    EXPECT_EQ(totpCounter(1234567890, 30, 0), 41134263u);
}

TEST_F(TotpCodecTest, CounterZeroAtEpoch)
{
    EXPECT_EQ(totpCounter(0, 30, 0), 0u);
}
