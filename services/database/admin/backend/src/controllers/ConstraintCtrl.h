/**
 * @file ConstraintCtrl.h
 * @brief Constraint listing endpoint.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief List table constraints.
class ConstraintCtrl
    : public drogon::HttpController<ConstraintCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(ConstraintCtrl::list,
                  "/api/constraints", drogon::Get,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief List constraints for a table.
    void list(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
