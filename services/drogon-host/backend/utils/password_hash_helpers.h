#pragma once
/**
 * @file password_hash_helpers.h
 * @brief Internal hex and PBKDF2 helpers for
 *        PasswordHash.
 */

#include <array>
#include <iomanip>
#include <openssl/evp.h>
#include <sstream>
#include <stdexcept>
#include <string>

namespace utils::detail
{

constexpr std::size_t kSaltBytes = 16;
constexpr std::size_t kDkBytes = 32;
constexpr int kIterations = 600'000;

/** @brief Convert raw bytes to a hex string. */
inline auto toHex(
    const unsigned char* data,
    std::size_t len) -> std::string
{
    std::ostringstream oss;
    for (std::size_t i = 0; i < len; ++i) {
        oss << std::hex << std::setfill('0')
            << std::setw(2)
            << static_cast<int>(data[i]);
    }
    return oss.str();
}

/** @brief Decode a hex string into raw bytes. */
inline auto fromHex(
    const std::string& hex,
    unsigned char* out,
    std::size_t maxLen) -> std::size_t
{
    std::size_t n = 0;
    for (std::size_t i = 0;
         i + 1 < hex.size() && n < maxLen;
         i += 2, ++n) {
        out[n] = static_cast<unsigned char>(
            std::stoi(hex.substr(i, 2),
                      nullptr, 16));
    }
    return n;
}

/** @brief Derive key via PBKDF2-HMAC-SHA-256. */
inline auto pbkdf2(
    const std::string& plain,
    const unsigned char* salt,
    std::size_t saltLen,
    int iters) -> std::string
{
    std::array<unsigned char, kDkBytes> dk{};
    int rc = PKCS5_PBKDF2_HMAC(
        plain.data(),
        static_cast<int>(plain.size()),
        salt, static_cast<int>(saltLen),
        iters, EVP_sha256(),
        static_cast<int>(kDkBytes),
        dk.data());
    if (rc != 1) {
        throw std::runtime_error(
            "PKCS5_PBKDF2_HMAC failed");
    }
    return toHex(dk.data(), kDkBytes);
}

} // namespace utils::detail
