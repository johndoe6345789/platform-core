/**
 * @file QueryCtrl.h
 * @brief Raw SQL and query builder endpoints.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief POST /api/query, /api/query/build.
class QueryCtrl
    : public drogon::HttpController<QueryCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(QueryCtrl::raw,
                  "/api/query", drogon::Post,
                  "pgadmin::SessionFilter");
    ADD_METHOD_TO(QueryCtrl::build,
                  "/api/query/build", drogon::Post,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Execute a raw SELECT query.
    void raw(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);

    /// @brief Build and execute a structured query.
    void build(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
