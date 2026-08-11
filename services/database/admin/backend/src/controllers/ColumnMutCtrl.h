/**
 * @file ColumnMutCtrl.h
 * @brief Column modify and drop endpoints.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief Modify and drop column DDL operations.
class ColumnMutCtrl
    : public drogon::HttpController<ColumnMutCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ColumnMutCtrl::modify,
                  "/api/columns", drogon::Put,
                  "pgadmin::SessionFilter");
    ADD_METHOD_TO(ColumnMutCtrl::drop,
                  "/api/columns", drogon::Delete,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Change a column's data type.
    void modify(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);

    /// @brief Drop a column from a table.
    void drop(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
