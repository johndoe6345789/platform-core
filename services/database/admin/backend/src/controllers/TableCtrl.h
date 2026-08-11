/**
 * @file TableCtrl.h
 * @brief Table listing endpoint.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief List all public-schema tables.
class TableCtrl
    : public drogon::HttpController<TableCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TableCtrl::list,
                  "/api/tables", drogon::Get,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief List all public-schema tables.
    void list(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
