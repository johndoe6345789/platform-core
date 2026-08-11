/**
 * @file test_passkey_attestation_auth_data.cpp
 * @brief Attested credential data parser checks.
 */

#include "auth/backend/passkeys/AttestationAuthData.h"

#include <gtest/gtest.h>

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace
{

std::vector<std::uint8_t> makeAuthData(std::uint8_t flags)
{
    std::vector<std::uint8_t> out(32, 0);
    out.push_back(flags);
    out.insert(out.end(), {0, 0, 0, 0});
    for (std::uint8_t i = 0; i < 16; ++i) out.push_back(i);
    out.insert(out.end(), {0, 3, 0xaa, 0xbb, 0xcc});
    out.push_back(0xa5);
    out.insert(out.end(), {0x01, 0x02, 0x03, 0x26, 0x20, 0x01});
    out.insert(out.end(), {0x21, 0x58, 0x20});
    out.insert(out.end(), 32, 0x11);
    out.insert(out.end(), {0x22, 0x58, 0x20});
    out.insert(out.end(), 32, 0x22);
    return out;
}

} // namespace

TEST(PasskeyAttestationAuthDataTest, AcceptsAttestedCredentialData)
{
    auto parsed = services::auth::passkeys::parseAttestedCredentialData(
        makeAuthData(0x41));
    EXPECT_EQ(parsed.credentialId.size(), 3u);
    EXPECT_EQ(parsed.publicKeyCose.size(), 77u);
}

TEST(PasskeyAttestationAuthDataTest, RejectsMissingAtFlag)
{
    EXPECT_THROW(
        services::auth::passkeys::parseAttestedCredentialData(
            makeAuthData(0x01)),
        std::runtime_error);
}

TEST(PasskeyAttestationAuthDataTest, RejectsTruncatedCredentialId)
{
    auto authData = makeAuthData(0x41);
    authData[53] = 0;
    authData[54] = 8;
    EXPECT_THROW(
        services::auth::passkeys::parseAttestedCredentialData(
            authData),
        std::runtime_error);
}
