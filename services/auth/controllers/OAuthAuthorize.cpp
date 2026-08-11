/**
 * @file OAuthAuthorize.cpp
 * @brief DEPRECATED: Keycloak migration phase 3.
 *
 * GET /api/auth/oauth/{provider}/authorize now 302-redirects
 * to Keycloak's authorize endpoint with kc_idp_hint set to
 * the requested provider. File retained per template-repo
 * policy; the OAuthClient/StateStore libraries are no longer
 * reachable from the route, but remain linkable.
 */

#include "OAuthController.h"

#include <drogon/HttpResponse.h>
#include <string>

namespace controllers
{

void OAuthController::authorize(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& provider)
{
    std::string url =
        "http://localhost:8889/sso/realms/nextra"
        "/protocol/openid-connect/auth"
        "?client_id=nextra-app&response_type=code"
        "&scope=openid+profile+email"
        "&redirect_uri=http%3A%2F%2Flocalhost%3A8889"
        "%2Fapp%2Fen%2Fauth%2Fcallback"
        "&kc_idp_hint=";
    url += provider;
    cb(drogon::HttpResponse::newRedirectionResponse(
        url, drogon::k302Found));
}

} // namespace controllers
