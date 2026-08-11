/**
 * @file QueryBuilder.h
 * @brief Builds parameterized SELECT queries.
 */

#pragma once

#include <json/json.h>
#include <string>

namespace pgadmin
{

/// @brief Result of building a query.
struct BuildResult {
    bool ok;
    std::string query;
    std::string error;
};

/// @brief Builds safe SELECT queries from JSON.
class QueryBuilder
{
  public:
    /// @brief Build a SELECT from JSON params.
    /// @param params JSON with table, columns,
    ///   where, orderBy, limit, offset
    /// @return BuildResult with query or error
    static BuildResult build(
        const Json::Value& params);
};

} // namespace pgadmin
