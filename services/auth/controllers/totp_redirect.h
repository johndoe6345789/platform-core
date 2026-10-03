#pragma once
/**
 * @file totp_redirect.h
 * @brief Shared 302 builder for the deprecated TOTP setup
 *        endpoints. Keycloak migration phase 3.
 */

#include "app_login_redirect.h"

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
    return appLoginRedirect();
}

} // namespace controllers
