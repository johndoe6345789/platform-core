/**
 * @file TableSchemaCtrl.h
 * @brief Table schema introspection endpoint.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief Column and primary key introspection.
class TableSchemaCtrl
    : public drogon::HttpController<
          TableSchemaCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(TableSchemaCtrl::schema,
                  "/api/tables/schema", drogon::Post,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Get column/PK info for a table.
    void schema(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
