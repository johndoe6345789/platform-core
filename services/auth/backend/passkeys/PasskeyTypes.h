#pragma once
/**
 * @file PasskeyTypes.h
 * @brief Shared POD types for the WebAuthn passkey flow.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::passkeys
{

/** @brief A registered credential as persisted in Postgres. */
struct Credential
{
    std::int64_t id{0};
    std::string userId;
    std::vector<std::uint8_t> credentialId;
    std::vector<std::uint8_t> publicKey;
    std::int64_t signCount{0};
    std::vector<std::string> transports;
    std::string nickname;
};

/** @brief Pending WebAuthn challenge stored server-side. */
struct PendingChallenge
{
    std::string userId;
    std::vector<std::uint8_t> challenge;
    bool isRegistration{true};
    std::int64_t createdAtSec{0};
};

/** @brief Parsed COSE public key (supports EC2 + RSA). */
struct CoseKeyParsed
{
    int kty{0};
    int alg{0};
    int crv{0};
    std::vector<std::uint8_t> x;
    std::vector<std::uint8_t> y;
    std::vector<std::uint8_t> n;
    std::vector<std::uint8_t> e;
};

} // namespace services::auth::passkeys
