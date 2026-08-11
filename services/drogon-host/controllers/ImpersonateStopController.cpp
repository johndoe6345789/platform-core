/**
 * @file ImpersonateStopController.cpp
 * @brief Stop-impersonate and status endpoints.
 *
 * stopImpersonate: swaps nextra_sso_admin back
 * into nextra_sso and clears the backup cookie.
 * status: returns whether impersonation is active.
 */

#include "ImpersonateController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/Cookie.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void ImpersonateController::stopImpersonate(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    auto backup =
        req->getCookie("nextra_sso_admin");
    if (backup.empty()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "Not impersonating"));
        return;
    }

    spdlog::info("Stopping impersonation");

    auto resp = ::utils::jsonOk(
        json{{"message", "Restored"}});

    drogon::Cookie sso("nextra_sso", backup);
    sso.setHttpOnly(true);
    sso.setPath("/");
    sso.setSameSite(
        drogon::Cookie::SameSite::kLax);
    sso.setMaxAge(30 * 24 * 3600);
    resp->addCookie(sso);

    drogon::Cookie clear("nextra_sso_admin", "");
    clear.setHttpOnly(true);
    clear.setPath("/");
    clear.setMaxAge(0);
    resp->addCookie(clear);

    cb(resp);
}

void ImpersonateController::status(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    auto backup =
        req->getCookie("nextra_sso_admin");
    json data = {
        {"impersonating", !backup.empty()},
    };
    cb(::utils::jsonOk(data));
}

} // namespace controllers
