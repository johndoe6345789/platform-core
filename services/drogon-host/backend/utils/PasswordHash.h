#pragma once
/**
 * @file PasswordHash.h
 * @brief Password hashing and verification utilities.
 *
 * Implements PBKDF2-HMAC-SHA-256 with a 16-byte cryptographically
 * random salt and 600 000 iterations (NIST SP 800-132).  Stored
 * format: "<saltHex>$<iterations>$<dkHex>".
 *
 * Comparison is performed via CRYPTO_memcmp to prevent timing attacks.
 */

#include <string>

namespace utils
{

/**
 * @brief Hash a plaintext password with PBKDF2-HMAC-SHA-256.
 *
 * Generates a fresh random salt on every call.
 *
 * @param plain  The raw password string.
 * @return Stored string in the format
 *         "<saltHex>$<iterations>$<dkHex>".
 * @throws std::runtime_error if the OpenSSL RNG or KDF fails.
 */
[[nodiscard]] auto hashPassword(const std::string& plain) -> std::string;

/**
 * @brief Verify a plaintext password against a stored PBKDF2 hash.
 *
 * Re-derives the key using the salt and iteration count embedded in
 * @p stored and compares with a timing-safe byte comparison.
 *
 * @param plain   The raw password to check.
 * @param stored  The stored "<saltHex>$<iterations>$<dkHex>" string.
 * @return True if the password matches, false otherwise.
 */
[[nodiscard]] auto verifyPassword(const std::string& plain,
                                  const std::string& stored) -> bool;

} // namespace utils
