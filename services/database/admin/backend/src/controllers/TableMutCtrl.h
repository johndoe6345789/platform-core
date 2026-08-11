/**
 * @file TableMutCtrl.h
 * @brief Create and drop table endpoints.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief POST/DELETE /api/tables (DDL).
class TableMutCtrl
    : public drogon::HttpController<TableMutCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TableMutCtrl::create,
                  "/api/tables/manage", drogon::Post,
                  "pgadmin::SessionFilter");
    ADD_METHOD_TO(TableMutCtrl::drop,
                  "/api/tables/manage",
                  drogon::Delete,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief CREATE TABLE with columns.
    void create(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);

    /// @brief DROP TABLE.
    void drop(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
