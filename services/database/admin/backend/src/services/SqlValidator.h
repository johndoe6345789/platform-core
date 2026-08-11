/**
 * @file SqlValidator.h
 * @brief SQL identifier and query validation.
 */

#pragma once

#include <json/json.h>
#include <regex>
#include <string>

namespace pgadmin
{

/// @brief Validates SQL identifiers and queries.
class SqlValidator
{
  public:
    /// @brief Check if name is a valid SQL id.
    /// @param name The identifier to validate
    /// @return true if safe to use in SQL
    static bool isValidIdentifier(
        const std::string& name)
    {
        static const std::regex pat(
            "^[a-zA-Z_][a-zA-Z0-9_]*$");
        return std::regex_match(name, pat);
    }

    /// @brief Check if a query is SELECT-only.
    /// @param sql The SQL string to check
    /// @return true if it contains no DDL/DML
    static bool isSelectOnly(
        const std::string& sql);

    /// @brief Escape a JSON value for SQL.
    /// @param val JSON value to sanitize
    /// @return SQL-safe string literal
    static std::string sanitizeValue(
        const Json::Value& val);
};

} // namespace pgadmin
