/**
 * @file RecordCtrl.h
 * @brief Record insert endpoint.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief POST /api/records (insert).
class RecordCtrl
    : public drogon::HttpController<RecordCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RecordCtrl::insert,
                  "/api/records", drogon::Post,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Insert a new record into a table.
    void insert(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
