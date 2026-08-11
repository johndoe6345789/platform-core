/**
 * @file UserBadges.cpp
 * @brief User badges retrieval endpoint.
 */

#include "UserController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon::orm;

namespace controllers
{

void UserController::getBadges(
    const drogon::HttpRequestPtr& /*req*/,
    Cb&& cb, const std::string& id)
{
    auto db = drogon::app().getDbClient();
    const std::string sql = R"(
        SELECT b.id, b.name, b.description,
               b.icon_url, ub.earned_at
        FROM user_badges ub
        JOIN badges b ON b.id = ub.badge_id
        WHERE ub.user_id = $1
        ORDER BY ub.earned_at DESC
    )";

    *db << sql << id
        >> [id, cb](const Result& r) {
            json badges = json::array();
            for (const auto& row : r) {
                badges.push_back(
                    {{"id", row["id"]
                          .as<std::string>()},
                     {"name", row["name"]
                          .as<std::string>()},
                     {"description",
                      row["description"]
                          .as<std::string>()},
                     {"earnedAt",
                      row["earned_at"]
                          .as<std::string>()}});
            }
            cb(::utils::jsonOk(
                {{"userId", id},
                 {"badges", badges}}));
        }
        >> [id, cb](
               const DrogonDbException& e) {
            spdlog::error("getBadges: {}",
                          e.base().what());
            cb(::utils::jsonOk(
                {{"userId", id},
                 {"badges", json::array()}}));
        };
}

} // namespace controllers
