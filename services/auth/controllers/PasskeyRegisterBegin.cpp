/**
 * @file PasskeyRegisterBegin.cpp
 * @brief DEPRECATED: Keycloak migration phase 3.
 *
 * Passkey registration is now handled by Keycloak's account
 * console action. The endpoint 302-redirects to Keycloak's
 * authorize URL with kc_action=webauthn-register-passwordless.
 * File retained per template-repo policy.
 */

#include "PasskeyController.h"
#include "passkey_redirect.h"

namespace controllers
{

void PasskeyController::registerBegin(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(passkeyKeycloakRedirect());
}

} // namespace controllers
