#pragma once
/**
 * @file app_login_redirect.h
 * @brief 302 to the app's login page, which starts the
 *        OIDC (dbal) authorization-code + PKCE flow.
 *
 * The deprecated login/register/password/OAuth/passkey/TOTP
 * endpoints used to send callers to Keycloak's hosted pages.
 * dbal has none of those, so they all land on the app's own
 * login page instead.
 */

#include <drogon/HttpResponse.h>

#include <cstdlib>
#include <string>

namespace controllers
{

/** @brief PUBLIC_ORIGIN without a trailing slash. */
inline std::string publicOrigin()
{
    const char* v = std::getenv("PUBLIC_ORIGIN");
    std::string o = v && *v ? v : "http://localhost:8889";
    while (!o.empty() && o.back() == '/') o.pop_back();
    return o;
}

/** @brief 302 to <PUBLIC_ORIGIN>/app/en/login. */
inline drogon::HttpResponsePtr appLoginRedirect()
{
    return drogon::HttpResponse::newRedirectionResponse(
        publicOrigin() + "/app/en/login", drogon::k302Found);
}

} // namespace controllers
