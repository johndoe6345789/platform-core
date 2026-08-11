/**
 * @file PasskeyAssertBegin.cpp
 * @brief DEPRECATED: Keycloak migration phase 3.
 *
 * Endpoint 302-redirects to Keycloak's account-action URL
 * for passkey enrolment. File retained per template-repo
 * policy.
 */

#include "PasskeyController.h"
#include "passkey_redirect.h"

namespace controllers
{

void PasskeyController::assertBegin(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(passkeyKeycloakRedirect());
}

} // namespace controllers
