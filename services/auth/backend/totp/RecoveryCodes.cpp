/**
 * @file RecoveryCodes.cpp
 * @brief Recovery code generation + SHA-256 hashing.
 */

#include "RecoveryCodes.h"

#include <openssl/rand.h>
#include <openssl/sha.h>

#include <array>
#include <cstdint>

namespace services::auth::totp
{

static const char* ALPHA =
    "ABCDEFGHJKLMNPQRSTUVWXYZ23456789";

std::vector<std::string> generateRecoveryCodes(
    int count, int length)
{
    std::vector<std::string> out;
    out.reserve(count);
    for (int i = 0; i < count; ++i) {
        std::string s;
        s.resize(length);
        std::vector<std::uint8_t> r(length);
        RAND_bytes(r.data(), length);
        for (int j = 0; j < length; ++j)
            s[j] = ALPHA[r[j] % 32];
        out.push_back(std::move(s));
    }
    return out;
}

std::string hashRecoveryCode(const std::string& plain)
{
    unsigned char d[SHA256_DIGEST_LENGTH];
    SHA256(
        reinterpret_cast<const unsigned char*>(plain.data()),
        plain.size(), d);
    static const char* hex = "0123456789abcdef";
    std::string out(SHA256_DIGEST_LENGTH * 2, '0');
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        out[i * 2] = hex[d[i] >> 4];
        out[i * 2 + 1] = hex[d[i] & 0xf];
    }
    return out;
}

int findMatchingRecoveryCode(
    const std::string& plain,
    const std::vector<std::string>& hashes)
{
    auto h = hashRecoveryCode(plain);
    for (std::size_t i = 0; i < hashes.size(); ++i)
        if (hashes[i] == h)
            return static_cast<int>(i);
    return -1;
}

} // namespace services::auth::totp
