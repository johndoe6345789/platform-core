/**
 * @file ImpersonateController.cpp
 * @brief Impersonate a user (admin only).
 *
 * Looks up the target user, generates tokens, and
 * sets the nextra_sso HttpOnly cookie so the admin
 * can browse the portal as that user.
 */

#include "ImpersonateController.h"
#include "impersonate_cookies.h"
#include "auth/backend/sso_session_lookup.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void ImpersonateController::impersonate(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    auto role = req->getAttributes()
        ->get<std::string>("user_role");
    if (role != "admin") {
        cb(::utils::jsonError(
            drogon::k403Forbidden, "Admin only"));
        return;
    }

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);

    if (body.is_discarded()
        || !body.contains("userId")) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "userId is required"));
        return;
    }

    auto adminId = req->getAttributes()
        ->get<std::string>("user_id");
    auto targetId =
        body["userId"].get<std::string>();

    auto adminCookie =
        req->getCookie("nextra_sso");

    spdlog::info(
        "Admin {} impersonating user {}",
        adminId, targetId);

    // Chosen mitigation: option (b) from the spec.
    // Issue a short-lived ACCESS token (not refresh)
    // as nextra_sso, 5 min, HttpOnly+Secure+Strict.
    // The admin refresh token is stashed only in the
    // backup cookie for stopImpersonate. Refresh
    // while impersonating is implicitly forbidden:
    // the cookie is an access token and
    // AuthTokenController::refresh only accepts
    // refresh tokens (JwtAuthFilter rejects access
    // tokens on the refresh path).
    services::ssoSessionLookup(
        targetId,
        [cb, adminCookie](const json& payload) {
            auto access =
                payload.value("accessToken", "");
            auto resp = ::utils::jsonOk(payload);
            resp->addCookie(
                makeImpersonationCookie(access));
            resp->addCookie(
                makeAdminBackupCookie(adminCookie));
            cb(resp);
        },
        [cb](drogon::HttpStatusCode code,
             const std::string& msg) {
            cb(::utils::jsonError(code, msg));
        });
}

} // namespace controllers
