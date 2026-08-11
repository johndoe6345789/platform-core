/**
 * @file Validators.cpp
 * @brief Input validation implementations using C++20.
 */

#include "Validators.h"

#include <algorithm>
#include <regex>

namespace utils
{

auto isValidEmail(const std::string& str) -> bool
{
    if (str.empty() || str.size() > 254) {
        return false;
    }
    // Basic RFC-5322-ish pattern; not exhaustive.
    static const std::regex kEmailRe{R"(^[a-zA-Z0-9.!#$%&'*+/=?^_`{|}~-]+)"
                                     R"(@[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61})"
                                     R"([a-zA-Z0-9])?)"
                                     R"((?:\.[a-zA-Z0-9](?:[a-zA-Z0-9-]{0,61})"
                                     R"([a-zA-Z0-9])?)*$)"};
    return std::regex_match(str, kEmailRe);
}

auto isValidUsername(const std::string& str) -> bool
{
    if (str.size() < 3 || str.size() > 30) {
        return false;
    }
    return std::ranges::all_of(str, [](char c) {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    });
}

auto isValidPassword(const std::string& str) -> bool
{
    if (str.size() < 8) {
        return false;
    }
    bool hasUpper = std::ranges::any_of(str, [](char c) {
        return std::isupper(static_cast<unsigned char>(c));
    });
    bool hasLower = std::ranges::any_of(str, [](char c) {
        return std::islower(static_cast<unsigned char>(c));
    });
    bool hasDigit = std::ranges::any_of(str, [](char c) {
        return std::isdigit(static_cast<unsigned char>(c));
    });
    return hasUpper && hasLower && hasDigit;
}

} // namespace utils
