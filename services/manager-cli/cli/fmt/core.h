/**
 * @file fmt/core.h
 * @brief Minimal fmt::print shim using C++20 std::format.
 *
 * Drop-in replacement for fmtlib so existing code that does
 * `#include <fmt/core.h>` and `fmt::print(...)` compiles
 * without the full fmtlib dependency.
 */

#pragma once

#include <cstdio>
#include <format>
#include <string>

namespace fmt
{

/**
 * @brief Format a string (delegates to std::format).
 * @tparam Args Format argument types.
 * @param f  Format string.
 * @param args  Arguments.
 * @return std::string The formatted string.
 */
template <typename... Args>
std::string format(std::format_string<Args...> f, Args&&... args)
{
    return std::format(f, std::forward<Args>(args)...);
}

/**
 * @brief Print a formatted string to stdout.
 * @tparam Args Format argument types.
 * @param f  Format string.
 * @param args  Arguments.
 */
template <typename... Args>
void print(std::format_string<Args...> f, Args&&... args)
{
    std::fputs(fmt::format(f, std::forward<Args>(args)...).c_str(), stdout);
}

} // namespace fmt
