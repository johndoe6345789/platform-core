/**
 * @file AuthTokenController.cpp
 * @brief Legacy token management: logout, refresh.
 *
 * Phase 4 of the Keycloak migration: Keycloak handles
 * refresh + logout natively (refresh-token grant, end-
 * session endpoint). These handlers are retained per
 * template-repo policy:
 *  - logout: clears the residual nextra_sso cookie and
 *    returns 200 so old SPA paths don't error.
 *  - refresh: returns 401 to force the SPA through
 *    Keycloak's refresh-token grant.
 *
 * The "me" endpoint is in AuthTokenMe.cpp.
 */

#include "AuthTokenController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/Cookie.h>
#include <drogon/HttpResponse.h>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AuthTokenController::logout(
    const drogon::HttpRequestPtr&, Cb&& cb)
{
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
