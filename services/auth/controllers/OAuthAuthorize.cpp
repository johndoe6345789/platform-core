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
#include "app_login_redirect.h"

#include <drogon/HttpResponse.h>
#include <string>

namespace controllers
{

void OAuthController::authorize(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& /*provider*/)
{
    cb(appLoginRedirect());
}

} // namespace controllers
