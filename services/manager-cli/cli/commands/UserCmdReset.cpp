/**
 * @file UserCmdReset.cpp
 * @brief Password reset SQL generator.
 */

#include "UserCmd.h"
#include "pbkdf2.h"

#include <fmt/core.h>

#include <fstream>
#include <iostream>
#include <string>

namespace manager
{

static std::string escU(const std::string& s)
{
    std::string o;
    o.reserve(s.size());
    for (char c : s)
        o += (c == '\'')
            ? "''" : std::string(1, c);
    return o;
}

int UserCmd::reset(
    const std::string& identifier,
    const std::string& password,
    const std::string& outFile)
{
    auto hash = pbkdf2::hash(password);

    std::ostream* os = &std::cout;
    std::ofstream file;
    if (!outFile.empty()) {
        file.open(outFile);
        os = &file;
    }

    *os << fmt::format(
        "-- Password reset for: {}\n"
        "UPDATE users\n"
        "SET password_hash = '{}',\n"
        "    updated_at    = NOW()\n"
        "WHERE username = '{}'\n"
        "   OR email    = '{}';\n",
        escU(identifier), escU(hash),
        escU(identifier), escU(identifier));

    std::cerr << "[user reset] SQL written for: "
              << identifier << "\n";
    return 0;
}

} // namespace manager
