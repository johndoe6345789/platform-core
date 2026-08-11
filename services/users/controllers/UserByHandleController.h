#pragma once
/**
 * @file UserByHandleController.h
 * @brief GET /api/users/by-handle/:handle endpoint.
 *
 * Resolves a public @handle to a user record.
 * JWT is optional — this is a public profile lookup.
 */

#include <drogon/HttpController.h>
#include <string>

namespace controllers
{

/**
 * @brief Look up a user by their unique handle.
 */
class UserByHandleController
    : public drogon::HttpController<
          UserByHandleController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        UserByHandleController::byHandle,
        "/api/users/by-handle/{handle}",
        drogon::Get);
    METHOD_LIST_END

    /**
     * @brief Resolve a handle to a user profile.
     * @param req    HTTP request.
     * @param cb     Response callback.
     * @param handle The @handle to look up.
     */
    void byHandle(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& handle);
};

} // namespace controllers
