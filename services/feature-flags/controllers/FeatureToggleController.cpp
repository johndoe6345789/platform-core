/**
 * @file FeatureToggleController.cpp
 * @brief Feature toggle endpoint implementations.
 */

#include "FeatureToggleController.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "feature_toggle_list.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon;
using namespace drogon::orm;

namespace controllers
{

void FeatureToggleController::list(
    const HttpRequestPtr& /*req*/, Cb&& cb)
{
    auto dbClient = app().getDbClient();
    *dbClient << "SELECT key, enabled, description, "
                 "scope FROM feature_toggles "
                 "ORDER BY key"
              >> [cb](const Result& r) {
                     cb(::utils::jsonOk(
                         {{"features",
                           featureRowsToJson(r)}}));
                 }
              >> [cb](const DrogonDbException& e) {
                     spdlog::error("features list: {}",
                                   e.base().what());
                     cb(::utils::jsonError(
                         k500InternalServerError,
                         "Internal server error"));
                 };
}

void FeatureToggleController::toggle(
    const HttpRequestPtr& req, Cb&& cb,
    const std::string& key)
{
    auto role = req->attributes()->get<
        std::string>("user_role");
    if (role != "admin") {
        cb(::utils::jsonError(
            k403Forbidden,
            "Admin access required"));
        return;
    }

    auto dbClient = app().getDbClient();
    *dbClient << "UPDATE feature_toggles "
                 "SET enabled = NOT enabled, "
                 "updated_at = NOW() "
                 "WHERE key = $1 "
                 "RETURNING key, enabled"
              << key
              >> [cb](const Result& r) {
                     if (r.empty()) {
                         cb(::utils::jsonError(
                             k404NotFound,
                             "Feature not found"));
                         return;
                     }
                     cb(::utils::jsonOk({
                         {"key",
                          r[0]["key"]
                              .as<std::string>()},
                         {"enabled",
                          r[0]["enabled"]
                              .as<bool>()},
                     }));
                 }
              >> [cb](const DrogonDbException& e) {
                     spdlog::error("toggle: {}",
                                   e.base().what());
                     cb(::utils::jsonError(
                         k500InternalServerError,
                         "Internal server error"));
                 };
}

} // namespace controllers
