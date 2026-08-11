#pragma once
/**
 * @file totp_redirect.h
 * @brief Shared 302 builder for the deprecated TOTP setup
 *        endpoints. Keycloak migration phase 3.
 */

#include <drogon/HttpResponse.h>

namespace controllers
{

/**
 * @brief Build a 302 redirect to Keycloak's authorize URL
 *        with kc_action=CONFIGURE_TOTP.
 * @return Drogon HTTP response pointer.
 */
inline drogon::HttpResponsePtr totpKeycloakRedirect()
{
    static constexpr const char* kUrl =
        "http://localhost:8889/sso/realms/nextra"
        "/protocol/openid-connect/auth"
        "?client_id=nextra-app&response_type=code"
        "&scope=openid"
        "&redirect_uri=http%3A%2F%2Flocalhost%3A8889"
        "%2Fapp%2Fen%2Fauth%2Fcallback"
        "&kc_action=CONFIGURE_TOTP";
    return drogon::HttpResponse::newRedirectionResponse(
        kUrl, drogon::k302Found);
}

} // namespace controllers
