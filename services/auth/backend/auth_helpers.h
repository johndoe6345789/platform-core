#pragma once
/**
 * @file auth_helpers.h
 * @brief Shared helper utilities for authentication services.
 */

#include "auth_service_types.h"

#include <cstddef>
#include <string>

namespace services
{

/**
 * @brief Convenience accessor for the default DB client.
 * @return Shared pointer to the Drogon ORM DB client.
 */
[[nodiscard]] auto authDb() -> DbClientPtr;

/**
 * @brief Validate email format with a simple regex.
 * @param email The email string to validate.
 * @return True if the format is valid.
 */
[[nodiscard]] auto
isValidAuthEmail(const std::string& email) -> bool;

/**
 * @brief Validate username (3-30 alphanumeric / underscore).
 * @param username The username to validate.
 * @return True if the username is acceptable.
 */
[[nodiscard]] auto
isValidAuthUsername(const std::string& username) -> bool;

/**
 * @brief Validate password strength.
 * @param password The plain-text password to check.
 * @return True if the password meets strength rules.
 */
[[nodiscard]] auto
isStrongAuthPassword(const std::string& password) -> bool;

/**
 * @brief Generate a cryptographically random hex token.
 * @param bytes Number of random bytes (default 32).
 * @return Hex-encoded random string.
 */
[[nodiscard]] auto
generateRandomToken(std::size_t bytes = 32) -> std::string;

} // namespace services
