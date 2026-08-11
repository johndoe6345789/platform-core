/**
 * @file AuthSsoSessionController.cpp
 * @brief Legacy cookie-to-session bootstrap endpoint.
 *
 * Phase 4 of the Keycloak migration: this endpoint used
 * to read the legacy `nextra_sso` HttpOnly cookie (in-
 * house HS256 token) and bootstrap a Redux session for
 * the SPA. Keycloak now owns auth end-to-end — the SPA
 * derives state directly from the Keycloak access token
 * via useKeycloak.
 *
 * The route is retained per template-repo policy
 * ("don't delete features"), but always returns 401 so
 * the SPA doesn't pick up a stale legacy session and
 * falls through to Keycloak login.
 */

#include "AuthSsoSessionController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AuthSsoSessionController::session(
    const drogon::HttpRequestPtr&, Cb&& cb)
{
    cb(::utils::jsonError(
        drogon::k401Unauthorized,
        "Legacy SSO session disabled — use Keycloak",
        "AUTH_006"));
}

}  // namespace controllers
