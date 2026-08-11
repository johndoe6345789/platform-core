/**
 * @file FlagsControllerAdmin.cpp
 * @brief Admin CRUD endpoints for feature flags.
 */

#include "FlagsController.h"
#include "flags_controller_json.h"
#include "feature-flags/backend/FlagStore.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon;

namespace controllers
{

static bool requireAdmin(
    const HttpRequestPtr& req, const Cb& cb)
{
    auto role = req->attributes()->get<
        std::string>("user_role");
    if (role == "admin") return true;
    cb(::utils::jsonError(
        k403Forbidden, "Admin access required"));
    return false;
}

void FlagsController::list(
    const HttpRequestPtr& req, Cb&& cb)
{
    if (!requireAdmin(req, cb)) return;
    try {
        services::flags::FlagStore store;
        auto rows = store.listAll();
        cb(::utils::jsonOk(
            {{"flags", flagsToJson(rows)}}));
    } catch (const std::exception& e) {
        spdlog::error("flags list: {}", e.what());
        cb(::utils::jsonError(
            k500InternalServerError, "list failed"));
    }
}

void FlagsController::patch(
    const HttpRequestPtr& req, Cb&& cb,
    const std::string& key)
{
    if (!requireAdmin(req, cb)) return;
    try {
        auto body = nlohmann::json::parse(
            std::string(req->getBody()),
            nullptr, false);
        if (body.is_discarded() || !body.is_object()) {
            cb(::utils::jsonError(
                k400BadRequest, "invalid body"));
            return;
        }
        auto actor = req->attributes()->get<
            std::string>("user_id");
        services::flags::FlagStore store;
        auto updated = store.update(
            key, parseFlagPatch(body), actor);
        if (!updated) {
            cb(::utils::jsonError(
                k404NotFound, "flag not found"));
            return;
        }
        cb(::utils::jsonOk(
            {{"flag", flagToJson(*updated)}}));
    } catch (const std::exception& e) {
        spdlog::error("flags patch: {}", e.what());
        cb(::utils::jsonError(
            k500InternalServerError, "patch failed"));
    }
}

void FlagsController::audit(
    const HttpRequestPtr& req, Cb&& cb,
    const std::string& key)
{
    if (!requireAdmin(req, cb)) return;
    services::flags::FlagStore store;
    auto flag = store.findByKey(key);
    if (!flag) {
        cb(::utils::jsonError(
            k404NotFound, "flag not found"));
        return;
    }
    cb(::utils::jsonOk({
        {"flag", flagToJson(*flag)},
        {"audit", store.audit(flag->id, 50)},
    }));
}

} // namespace controllers
