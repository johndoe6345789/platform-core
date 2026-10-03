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
#include "app_login_redirect.h"

#include <drogon/HttpResponse.h>
#include <string>

namespace controllers
{

void AuthController::registerUser(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(appLoginRedirect());
}

} // namespace controllers
