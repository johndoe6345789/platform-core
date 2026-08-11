/**
 * @file TableDataCtrl.h
 * @brief Table data read endpoint.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief Fetch rows from a table.
class TableDataCtrl
    : public drogon::HttpController<TableDataCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TableDataCtrl::data,
                  "/api/tables/data", drogon::Post,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Fetch first 100 rows of a table.
    void data(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
