/**
 * @file AuthTokenController.cpp
 * @brief Legacy token management: logout, refresh.
 *
 * Phase 4 of the Keycloak migration: Keycloak handles
 * refresh + logout natively (refresh-token grant, end-
 * session endpoint). These handlers are retained per
 * template-repo policy:
 *  - logout: revokes the DB session (9.1 hardening),
 *    clears the residual nextra_sso cookie, returns 200.
 *  - refresh: returns 401 to force the SPA through
 *    Keycloak's refresh-token grant.
 *
 * The "me" endpoint is in AuthTokenMe.cpp.
 */

#include "AuthTokenController.h"
#include "auth/backend/SessionStore.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/Cookie.h>
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>
#include <string>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AuthTokenController::logout(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    // Revoke the DB session so the JTI is immediately
    // invalid for future requests (9.1 hardening).
    const auto jti =
        req->attributes()->get<std::string>("jti");
    if (!jti.empty()) {
        services::auth::SessionStore::revokeSession(
            jti,
            [](bool) {},
            [jti](const std::string& e) {
                spdlog::error(
                    "logout revoke jti={}: {}",
                    jti, e);
            });
    }

    auto resp = ::utils::jsonOk(
        {{"message", "Logged out"}});
    // Clear any residual legacy SSO cookie. New auth
    // state lives in the Keycloak end-session flow.
    drogon::Cookie sso("nextra_sso", "");
    sso.setHttpOnly(true);
    sso.setPath("/");
    sso.setMaxAge(0);
    resp->addCookie(sso);
    cb(resp);
}

void AuthTokenController::refresh(
    const drogon::HttpRequestPtr&, Cb&& cb)
{
    cb(::utils::jsonError(
        drogon::k401Unauthorized,
        "Legacy refresh disabled — use Keycloak",
        "AUTH_005"));
}

}  // namespace controllers
