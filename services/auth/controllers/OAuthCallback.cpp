/**
 * @file OAuthCallback.cpp
 * @brief DEPRECATED: Keycloak migration phase 3.
 *
 * The in-house OAuth callback no longer exchanges codes;
 * Keycloak's broker performs the exchange. Stragglers that
 * still hit this route are 302-redirected to the app login
 * page. File retained per template-repo policy.
 */

#include "OAuthController.h"
#include "app_login_redirect.h"

#include <drogon/HttpResponse.h>
#include <string>

namespace controllers
{

void OAuthController::callback(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& /*provider*/)
{
    cb(appLoginRedirect());
}

} // namespace controllers
