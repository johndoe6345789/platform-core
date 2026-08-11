#pragma once
/**
 * @file Validators.h
 * @brief Input validation utilities.
 */

#include <string>

namespace utils
{

/**
 * @brief Check if a string is a valid email address.
 * @param str  Input to validate.
 * @return True if the input matches a basic email pattern.
 */
[[nodiscard]] auto isValidEmail(const std::string& str) -> bool;

/**
 * @brief Check if a string is a valid username.
 *
 * Rules: alphanumeric + underscore, 3-30 characters.
 *
 * @param str  Input to validate.
 * @return True if the input is a valid username.
 */
[[nodiscard]] auto isValidUsername(const std::string& str) -> bool;

/**
 * @brief Check if a string is a valid password.
 *
 * Rules: minimum 8 characters, at least one uppercase,
 * one lowercase, and one digit.
 *
 * @param str  Input to validate.
 * @return True if the input meets password requirements.
 */
[[nodiscard]] auto isValidPassword(const std::string& str) -> bool;

} // namespace utils
