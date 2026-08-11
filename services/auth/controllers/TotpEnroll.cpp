/**
 * @file TotpEnroll.cpp
 * @brief DEPRECATED: Keycloak migration phase 3.
 *
 * TOTP enrolment is now a Keycloak account action. Endpoint
 * 302-redirects to the authorize URL with
 * kc_action=CONFIGURE_TOTP. File retained per template-repo
 * policy.
 */

#include "TotpController.h"
#include "totp_redirect.h"

namespace controllers
{

void TotpController::enroll(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(totpKeycloakRedirect());
}

} // namespace controllers
