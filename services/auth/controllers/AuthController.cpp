/**
 * @file AuthController.cpp
 * @brief DEPRECATED registerUser: Keycloak migration phase 3.
 *
 * The legacy registration endpoint now 302-redirects to
 * Keycloak's hosted registration page. Login remains in
 * AuthLogin.cpp (legacy token flow still supported during
 * the soak window). File retained per template-repo policy.
 */

#include "AuthController.h"

#include <drogon/HttpResponse.h>
#include <string>

namespace controllers
{

void AuthController::registerUser(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    static constexpr const char* kKcRegister =
        "http://localhost:8889/sso/realms/nextra"
        "/protocol/openid-connect/registrations"
        "?client_id=nextra-app&response_type=code"
        "&scope=openid+profile+email"
        "&redirect_uri=http%3A%2F%2Flocalhost%3A8889"
        "%2Fapp%2Fen%2Fauth%2Fcallback";
    cb(drogon::HttpResponse::newRedirectionResponse(
        kKcRegister, drogon::k302Found));
}

} // namespace controllers
