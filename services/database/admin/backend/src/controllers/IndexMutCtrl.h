/**
 * @file IndexMutCtrl.h
 * @brief Index create and drop endpoints.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief Create and drop indexes.
class IndexMutCtrl
    : public drogon::HttpController<IndexMutCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(IndexMutCtrl::create,
                  "/api/indexes", drogon::Post,
                  "pgadmin::SessionFilter");
    ADD_METHOD_TO(IndexMutCtrl::drop,
                  "/api/indexes", drogon::Delete,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Create an index on a table.
    void create(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);

    /// @brief Drop an index by name.
    void drop(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
