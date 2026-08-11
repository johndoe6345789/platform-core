/**
 * @file IndexCtrl.h
 * @brief Index listing endpoint.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief List indexes for a table.
class IndexCtrl
    : public drogon::HttpController<IndexCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(IndexCtrl::list,
                  "/api/indexes", drogon::Get,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief List all indexes on a table.
    void list(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
