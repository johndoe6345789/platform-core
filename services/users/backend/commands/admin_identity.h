#pragma once

#include <string>

namespace commands::admin
{

/**
 * @brief Derive a valid username candidate from an email address.
 * @param email Source email address.
 * @return Lower-case username using only alnum and underscores.
 */
[[nodiscard]] auto usernameFromEmail(
    const std::string& email) -> std::string;

} // namespace commands::admin
