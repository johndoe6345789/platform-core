#pragma once
/**
 * @file AuthTokenController.h
 * @brief Token-management endpoints: logout, refresh, me.
 */

#include <drogon/HttpController.h>
#include <string>

namespace controllers
{

class AuthTokenController
    : public drogon::HttpController<AuthTokenController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthTokenController::logout,
                  "/api/auth/logout", drogon::Post,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(AuthTokenController::refresh,
                  "/api/auth/refresh", drogon::Post);
    ADD_METHOD_TO(AuthTokenController::me,
                  "/api/auth/me", drogon::Get,
                  "filters::JwtAuthFilter");
    METHOD_LIST_END

    /** @brief Invalidate the current access token. */
    void logout(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Issue a new access token via refresh token. */
    void refresh(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Return the authenticated user's profile. */
    void me(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
