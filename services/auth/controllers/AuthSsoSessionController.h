#pragma once
/**
 * @file AuthSsoSessionController.h
 * @brief Cookie-to-session bootstrap endpoint.
 *
 * GET /api/auth/sso-session
 *   Reads the `nextra_sso` HttpOnly cookie, validates the
 *   refresh token, and returns a fresh access token plus
 *   the user profile — so the main app never needs to
 *   store tokens in localStorage.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class AuthSsoSessionController
    : public drogon::HttpController<
          AuthSsoSessionController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        AuthSsoSessionController::session,
        "/api/auth/sso-session",
        drogon::Get);
    METHOD_LIST_END

    /**
     * @brief Bootstrap an app session from the SSO cookie.
     * @param req HTTP request (must carry nextra_sso cookie).
     * @param cb  Response callback.
     */
    void session(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
