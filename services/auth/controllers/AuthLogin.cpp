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
 * any caller is now 302'd to Keycloak's authorize
 * endpoint to enrol them in the proper PKCE flow.
 */

#include "AuthController.h"

#include <drogon/HttpResponse.h>
#include <string>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AuthController::login(
    const drogon::HttpRequestPtr&, Cb&& cb)
{
    static const std::string kTarget =
        "http://localhost:8889"
        "/sso/realms/nextra/"
        "protocol/openid-connect/auth"
        "?client_id=nextra-app"
        "&response_type=code"
        "&scope=openid+profile+email"
        "&redirect_uri="
        "http%3A%2F%2Flocalhost%3A8889"
        "%2Fapp%2Fen%2Fauth%2Fcallback";
    auto resp = drogon::HttpResponse::
        newRedirectionResponse(kTarget,
            drogon::k302Found);
    cb(resp);
}

}  // namespace controllers
