/**
 * @file auth_helpers.cpp
 * @brief Shared helper utilities for authentication services.
 */

#include "auth/backend/auth_helpers.h"

#include <drogon/drogon.h>
#include <fmt/format.h>

#include <algorithm>
#include <cctype>
#include <random>
#include <regex>
#include <string>
#include <vector>

namespace services
{

auto authDb() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

auto isValidAuthEmail(const std::string& email) -> bool
{
    static const std::regex kPattern(
        R"(^[a-zA-Z0-9._%+\-]+@[a-zA-Z0-9.\-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, kPattern);
}

auto isValidAuthUsername(const std::string& uname) -> bool
{
    if (uname.size() < 3 || uname.size() > 30) {
        return false;
    }
    return std::ranges::all_of(uname, [](char c) {
        return std::isalnum(
                   static_cast<unsigned char>(c)) ||
               c == '_';
    });
}

auto isStrongAuthPassword(const std::string& pw) -> bool
{
    if (pw.size() < 8)
        return false;
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    for (char c : pw) {
        auto uc = static_cast<unsigned char>(c);
        if (std::isupper(uc))
            hasUpper = true;
        if (std::islower(uc))
            hasLower = true;
        if (std::isdigit(uc))
            hasDigit = true;
    }
    return hasUpper && hasLower && hasDigit;
}

auto generateRandomToken(std::size_t bytes) -> std::string
{
    std::vector<unsigned char> buf(bytes);
    std::random_device rd;
    std::ranges::generate(buf, [&rd]() {
        return static_cast<unsigned char>(rd() & 0xFF);
    });
    std::string hex;
    hex.reserve(bytes * 2);
    for (auto b : buf) {
        hex += fmt::format("{:02x}", b);
    }
    return hex;
}

} // namespace services
