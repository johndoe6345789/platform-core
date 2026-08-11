/**
 * @file AuthValidateController.cpp
 * @brief SSO token-validation endpoint for nginx
 *        auth_request.
 *
 * Reads the `nextra_sso` cookie which carries a
 * Keycloak-issued RS256 access token. Returns 200 with
 * X-User-Id / X-User-Email / X-User-Roles headers on
 * success, 401 otherwise.
 *
 * The legacy in-house HS256 fallback was removed in
 * Phase 4 of the Keycloak migration.
 */

#include "AuthValidateController.h"
#include "auth/backend/keycloak/KeycloakVerifier.h"
#include "auth/backend/keycloak/UserProvision.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/HttpResponse.h>
#include <string>
#include <string_view>
#include <vector>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

static drogon::HttpResponsePtr buildOk(
    const std::string& userId,
    const std::string& email,
    const std::string& roles)
{
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->addHeader("X-User-Id", userId);
    if (!email.empty())
        resp->addHeader("X-User-Email", email);
    if (!roles.empty())
        resp->addHeader("X-User-Roles", roles);
    return resp;
}

static std::string joinRoles(
    const std::vector<std::string>& roles)
{
    std::string out;
    for (const auto& r : roles) {
        if (!out.empty()) out += ',';
        out += r;
    }
    return out;
}

void AuthValidateController::validate(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    // Credential may arrive as the SSO cookie (browser /
    // nginx auth_request) or as a Bearer header (forwarded
    // by another service's http-filters auth_client).
    auto token = req->getCookie("nextra_sso");
    if (token.empty()) {
        const auto h = req->getHeader("Authorization");
        constexpr std::string_view kBearer = "Bearer ";
        if (h.size() > kBearer.size()
            && h.compare(0, kBearer.size(), kBearer) == 0)
            token = h.substr(kBearer.size());
    }
    if (token.empty()) {
        cb(::utils::jsonError(
            drogon::k401Unauthorized,
            "No credential", "AUTH_006"));
        return;
    }

    auto kc = services::auth::keycloak::defaultVerifier()
                  .verify(token);
    if (!kc) {
        cb(::utils::jsonError(
            drogon::k401Unauthorized,
            "Invalid or expired SSO token",
            "AUTH_005"));
        return;
    }

    services::auth::keycloak::ensureUserRow(*kc);
    cb(buildOk(kc->sub, kc->email,
               joinRoles(kc->roles)));
}

}  // namespace controllers
