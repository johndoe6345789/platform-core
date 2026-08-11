/**
 * @file StatusController.cpp
 * @brief StatusController::incidents — open incident list.
 */

#include "status-page/controllers/StatusController.h"
#include "status-page/controllers/status_controller_helpers.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace nextra::controllers
{

using namespace drogon;
using namespace drogon::orm;

void StatusController::incidents(
    const HttpRequestPtr& /*req*/,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto db = drogon::app().getDbClient();
    const std::string sql = R"(
        SELECT id, title, body, severity, status,
               started_at, resolved_at
        FROM incidents
        WHERE status <> 'resolved'
        ORDER BY started_at DESC
        LIMIT 50
    )";
    *db << sql >> [cb](const Result& r) {
        nlohmann::json out = nlohmann::json::array();
        for (const auto& row : r) {
            out.push_back(
                status_helpers::rowToJson(row));
        }
        cb(::utils::jsonOk({{"incidents", out}}));
    } >> [cb](const DrogonDbException& e) {
        spdlog::error(
            "status incidents error: {}",
            e.base().what());
        cb(::utils::jsonError(
            k500InternalServerError,
            "status unavailable"));
    };
}

} // namespace nextra::controllers
