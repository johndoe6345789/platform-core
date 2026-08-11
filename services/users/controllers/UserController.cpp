/**
 * @file UserController.cpp
 * @brief User list endpoint (admin-only, paginated).
 */

#include "UserController.h"
#include "user_list_helpers.h"
#include "user_list_query.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "drogon-host/backend/utils/parse_helpers.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using namespace drogon::orm;

namespace controllers
{

void UserController::list(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto role = req->attributes()
        ->get<std::string>("user_role");
    if (role != "admin") {
        cb(::utils::jsonError(
            drogon::k403Forbidden,
            "Admin access required"));
        return;
    }

    auto pageStr = req->getParameter("page");
    auto ppStr =
        req->getParameter("per_page");
    int64_t page =
        ::utils::safeStoll(pageStr, 1);
    int64_t perPage =
        ::utils::safeStoll(ppStr, 20);
    int64_t offset = (page - 1) * perPage;

    auto db = drogon::app().getDbClient();

    *db << std::string(kUserCountSql)
        >> [db, page, perPage, offset, cb](
               const Result& cr) {
            int64_t total =
                cr[0]["total"].as<int64_t>();
            *db << std::string(kUserListSql)
                << perPage << offset
                >> [total, page, perPage, cb](
                       const Result& r) {
                    json users = json::array();
                    for (const auto& row : r)
                        users.push_back(
                            detail::userFromRow(
                                row));
                    cb(::utils::jsonPaginated(
                        users, total, page,
                        perPage));
                }
                >> [cb](const DrogonDbException&
                            e) {
                    spdlog::error(
                        "user list: {}",
                        e.base().what());
                    cb(::utils::jsonError(
                        drogon::k500InternalServerError,
                        "Failed to list users"));
                };
        }
        >> [cb](const DrogonDbException& e) {
            spdlog::error("user count: {}",
                          e.base().what());
            cb(::utils::jsonError(
                drogon::k500InternalServerError,
                "Failed to list users"));
        };
}

} // namespace controllers
