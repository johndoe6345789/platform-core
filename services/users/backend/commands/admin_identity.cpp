/**
 * @file admin_identity.cpp
 * @brief Username derivation for create-admin.
 */

#include "admin_identity.h"

#include <algorithm>
#include <cctype>

namespace commands::admin
{

auto usernameFromEmail(const std::string& email) -> std::string
{
    auto local = email.substr(0, email.find('@'));
    std::string out;
    for (char c : local) {
        auto uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc)) {
            out += static_cast<char>(std::tolower(uc));
        } else if (out.empty() || out.back() != '_') {
            out += '_';
        }
    }
    while (!out.empty() && out.back() == '_') out.pop_back();
    if (out.size() < 3) out = "admin";
    if (out.size() > 30) out.resize(30);
    return out;
}

} // namespace commands::admin
