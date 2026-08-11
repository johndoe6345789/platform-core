/**
 * @file pbkdf2.h
 * @brief PBKDF2-SHA-256 helpers for the manager CLI.
 *
 * Mirrors backend/src/utils/password_hash_helpers.h
 * so that seed generation uses the exact same algorithm
 * as the API server. Format: saltHex$iters$dkHex.
 */

#pragma once

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>

namespace manager::pbkdf2
{

constexpr std::size_t kSaltBytes  = 16;
constexpr std::size_t kDkBytes    = 32;
constexpr int         kIterations = 600'000;

inline std::string toHex(
    const unsigned char* d, std::size_t n)
{
    std::ostringstream ss;
    for (std::size_t i = 0; i < n; ++i)
        ss << std::hex << std::setfill('0')
           << std::setw(2)
           << static_cast<int>(d[i]);
    return ss.str();
}

/** @brief Derive key via PKCS5-PBKDF2-HMAC-SHA256. */
inline std::string derive(
    const std::string& plain,
    const unsigned char* salt,
    std::size_t saltLen,
    int iters)
{
    std::array<unsigned char, kDkBytes> dk{};
    int rc = PKCS5_PBKDF2_HMAC(
        plain.data(),
        static_cast<int>(plain.size()),
        salt,
        static_cast<int>(saltLen),
        iters,
        EVP_sha256(),
        static_cast<int>(kDkBytes),
        dk.data());
    if (rc != 1)
        throw std::runtime_error(
            "PKCS5_PBKDF2_HMAC failed");
    return toHex(dk.data(), kDkBytes);
}

/**
 * @brief Hash a plaintext password.
 * @param plain  Plaintext password.
 * @return Stored hash: saltHex$600000$dkHex.
 */
inline std::string hash(const std::string& plain)
{
    std::array<unsigned char, kSaltBytes> salt{};
    if (RAND_bytes(
            salt.data(),
            static_cast<int>(kSaltBytes)) != 1)
        throw std::runtime_error("RAND_bytes failed");
    return toHex(salt.data(), kSaltBytes)
        + "$" + std::to_string(kIterations)
        + "$" + derive(
            plain, salt.data(), kSaltBytes,
            kIterations);
}

} // namespace manager::pbkdf2
