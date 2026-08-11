/**
 * @file SqlValidator.cpp
 * @brief SQL identifier and query validation.
 */

#include "SqlValidator.h"

#include <algorithm>
#include <array>
#include <cctype>

namespace pgadmin
{

bool SqlValidator::isSelectOnly(
    const std::string& sql)
{
    std::string upper;
    upper.reserve(sql.size());
    for (char c : sql)
        upper += static_cast<char>(
            std::toupper(
                static_cast<unsigned char>(c)));

    static const std::array<std::string, 8>
        blocked = {
            "INSERT", "UPDATE", "DELETE", "DROP",
            "CREATE", "ALTER", "TRUNCATE", "GRANT",
        };

    for (const auto& kw : blocked) {
        if (upper.find(kw) != std::string::npos)
            return false;
    }
    return true;
}

std::string SqlValidator::sanitizeValue(
    const Json::Value& val)
{
    if (val.isNull()) return "NULL";
    if (val.isBool())
        return val.asBool() ? "TRUE" : "FALSE";
    if (val.isNumeric())
        return std::to_string(val.asDouble());
    auto s = val.asString();
    std::string out = "'";
    for (char c : s) {
        if (c == '\'') out += "''";
        else out += c;
    }
    return out + "'";
}

} // namespace pgadmin
