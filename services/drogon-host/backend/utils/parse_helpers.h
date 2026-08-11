#pragma once
/**
 * @file parse_helpers.h
 * @brief Header-only numeric parsing helpers used across
 *        multiple controllers.
 */

#include <string>

namespace utils
{

/**
 * @brief Safely parse a string to long long.
 * @param s        String to parse.
 * @param fallback Value returned when parsing fails.
 * @return Parsed value or @p fallback on error.
 */
inline auto safeStoll(const std::string& s,
                      long long fallback) noexcept
    -> long long
{
    try {
        return std::stoll(s);
    } catch (...) {
        return fallback;
    }
}

} // namespace utils
