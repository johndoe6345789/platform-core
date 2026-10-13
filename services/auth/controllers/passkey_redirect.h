#pragma once
/**
 * @file passkey_redirect.h
 * @brief Shared 302 builder for the four deprecated passkey
 *        endpoints. Keycloak migration phase 3.
 */

#include "app_login_redirect.h"

#include <drogon/HttpResponse.h>

namespace controllers
{

/**
 * @brief Build a 302 redirect to Keycloak's authorize URL
 *        with kc_action=webauthn-register-passwordless.
 * @return Drogon HTTP response pointer.
 */
inline drogon::HttpResponsePtr passkeyKeycloakRedirect()
{
    return appLoginRedirect();
}

} // namespace controllers
