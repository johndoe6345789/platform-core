/**
 * @file RecordUpdateCtrl.h
 * @brief Record update and delete endpoints.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief PUT/DELETE /api/records.
class RecordUpdateCtrl
    : public drogon::HttpController<RecordUpdateCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(RecordUpdateCtrl::update,
                  "/api/records", drogon::Put,
                  "pgadmin::SessionFilter");
    ADD_METHOD_TO(RecordUpdateCtrl::remove,
                  "/api/records", drogon::Delete,
                  "pgadmin::SessionFilter");
    METHOD_LIST_END

    /// @brief Update a record by primary key.
    void update(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);

    /// @brief Delete a record by primary key.
    void remove(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
