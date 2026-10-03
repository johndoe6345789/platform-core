/**
 * @file MentorController.cpp
 * @brief Toggle mentor opt-in for the authenticated user.
 */

#include "MentorController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb   = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon::orm;

namespace controllers
{

void MentorController::toggleMentor(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto userId = req->attributes()
        ->get<std::string>("user_id");
    if (userId.empty()) {
        cb(::utils::jsonError(
            drogon::k401Unauthorized,
            "Authentication required"));
        return;
    }

    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);
    if (body.is_discarded()
        || !body.contains("is_mentor")
        || !body["is_mentor"].is_boolean()) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "Field is_mentor (bool) required"));
        return;
    }

    bool isMentor = body["is_mentor"].get<bool>();
    auto db = drogon::app().getDbClient();

    const std::string sql = R"(
        UPDATE users
        SET is_mentor = $1, updated_at = NOW()
        WHERE id = $2
        RETURNING id, is_mentor
    )";

    *db << sql << isMentor << userId
        >> [cb](const Result& r) {
            if (r.empty()) {
                cb(::utils::jsonError(
                    drogon::k404NotFound,
                    "User not found"));
                return;
            }
            cb(::utils::jsonOk({
                {"id",
                 r[0]["id"].as<std::string>()},
                {"is_mentor",
                 r[0]["is_mentor"].as<bool>()}}));
        }
        >> [cb](const DrogonDbException& e) {
            spdlog::error("toggleMentor: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Internal error"));
        };
}

} // namespace controllers
