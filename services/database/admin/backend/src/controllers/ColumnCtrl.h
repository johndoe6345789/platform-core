/**
 * @file ColumnCtrl.h
 * @brief Add column endpoint.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief Add a column to a table.
class ColumnCtrl
    : public drogon::HttpController<ColumnCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ColumnCtrl::add,
                  "/api/columns", drogon::Post,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Add a new column to a table.
    void add(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
