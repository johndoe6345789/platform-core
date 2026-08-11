/**
 * @file PasswordHash.cpp
 * @brief PBKDF2-SHA-256 password hashing
 *        implementation.
 *
 * Stored format: "<saltHex>$<iterations>$<dkHex>".
 * Uses 600 000 iterations (NIST SP 800-132) and
 * CRYPTO_memcmp for timing-safe verification.
 */

#include "PasswordHash.h"
#include "password_hash_helpers.h"

#include <array>
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <stdexcept>

namespace utils
{

using detail::fromHex;
using detail::kDkBytes;
using detail::kIterations;
using detail::kSaltBytes;
using detail::pbkdf2;
using detail::toHex;

auto hashPassword(
    const std::string& plain) -> std::string
{
    std::array<unsigned char, kSaltBytes> salt{};
    if (RAND_bytes(salt.data(),
            static_cast<int>(kSaltBytes)) != 1) {
        throw std::runtime_error(
            "RAND_bytes failed");
    }
    auto saltHex = toHex(
        salt.data(), kSaltBytes);
    auto dkHex = pbkdf2(
        plain, salt.data(),
        kSaltBytes, kIterations);
    return saltHex + "$"
        + std::to_string(kIterations)
        + "$" + dkHex;
}

auto verifyPassword(
    const std::string& plain,
    const std::string& stored) -> bool
{
    auto p1 = stored.find('$');
    if (p1 == std::string::npos) return false;
    auto p2 = stored.find('$', p1 + 1);
    if (p2 == std::string::npos) return false;

    auto saltHex = stored.substr(0, p1);
    int iters = std::stoi(
        stored.substr(p1 + 1, p2 - p1 - 1));
    auto storedDk = stored.substr(p2 + 1);

    std::array<unsigned char, kSaltBytes> salt{};
    fromHex(saltHex, salt.data(), kSaltBytes);

    auto candidateDk = pbkdf2(
        plain, salt.data(), kSaltBytes, iters);

    return CRYPTO_memcmp(
        candidateDk.data(),
        storedDk.data(),
        candidateDk.size()) == 0;
}

} // namespace utils
