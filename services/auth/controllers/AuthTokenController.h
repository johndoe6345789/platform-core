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
    ADD_METHOD_TO(
        AuthTokenController::registerSession,
        "/api/auth/session", drogon::Post);
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

    /**
     * @brief Register a Keycloak JTI in the session store.
     *
     * Called by the SPA right after every token exchange or
     * silent refresh. No JwtAuthFilter — validates the token
     * internally, then upserts into user_sessions so that
     * subsequent requests pass the session-hardening check.
     */
    void registerSession(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
