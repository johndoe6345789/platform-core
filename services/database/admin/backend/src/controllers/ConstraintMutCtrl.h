/**
 * @file ConstraintMutCtrl.h
 * @brief Constraint add and drop endpoints.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief Add and drop table constraints.
class ConstraintMutCtrl
    : public drogon::HttpController<
          ConstraintMutCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ConstraintMutCtrl::add,
                  "/api/constraints", drogon::Post,
                  "pgadmin::SessionFilter");
    ADD_METHOD_TO(ConstraintMutCtrl::drop,
                  "/api/constraints",
                  drogon::Delete,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Add a constraint to a table.
    void add(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);

    /// @brief Drop a constraint from a table.
    void drop(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
