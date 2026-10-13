/**
 * @file AuthLogin.cpp
 * @brief Legacy email/password login endpoint.
 *
 * Phase 4 of the Keycloak migration: Keycloak owns the
 * login flow end-to-end. This endpoint used to verify a
 * password and issue an in-house JWT + set the legacy
 * `nextra_sso` HttpOnly cookie carrying the refresh
 * token. That bypassed Keycloak entirely.
 *
 * The route is retained per template-repo policy but
 * any caller is now 302'd to the app's login page, which
 * runs the OIDC PKCE flow.
 */

#include "AuthController.h"
#include "app_login_redirect.h"

#include <drogon/HttpResponse.h>
#include <string>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AuthController::login(
    const drogon::HttpRequestPtr&, Cb&& cb)
{
    auto resp = appLoginRedirect();
    cb(resp);
}

}  // namespace controllers
