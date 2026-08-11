/**
 * @file UserSearchService.cpp
 * @brief Implementation of paginated user listing.
 */

#include "users/backend/UserSearchService.h"
#include "users/backend/UserLookupService.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto UserSearchService::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

void UserSearchService::listUsers(std::int32_t page, std::int32_t perPage,
                                  Callback onSuccess, ErrCallback onError)
{
    if (page < 1)
        page = 1;
    if (perPage < 1)
        perPage = 20;
    if (perPage > 100)
        perPage = 100;

    auto dbClient = db();
    auto offset = static_cast<std::int64_t>(page - 1) * perPage;

    const std::string countSql = R"(
        SELECT COUNT(*) AS total FROM users
    )";

    *dbClient << countSql >> [dbClient, page, perPage, offset, onSuccess,
                              onError](const Result& countResult) {
        std::int64_t total = 0;
        if (!countResult.empty()) {
            total = countResult[0]["total"].as<std::int64_t>();
        }

        const std::string dataSql = R"(
                SELECT id, email, username, display_name,
                       role, email_confirmed, total_points,
                       current_streak, avatar_url, bio,
                       created_at
                FROM users
                ORDER BY created_at DESC
                LIMIT $1 OFFSET $2
            )";

        *dbClient << dataSql << perPage << offset >>
            [total, page, perPage, onSuccess](const Result& result) {
                json items = json::array();
                for (const auto& row : result) {
                    items.push_back(UserLookupService::rowToJson(row));
                }
                std::int64_t totalPages = (total + perPage - 1) / perPage;
                onSuccess({{"data", items},
                           {"pagination",
                            {{"total", total},
                             {"page", page},
                             {"perPage", perPage},
                             {"totalPages", totalPages}}}});
            } >>
            [onError](const DrogonDbException& e) {
                spdlog::error("listUsers data DB error: {}", e.base().what());
                onError(k500InternalServerError, "Internal server error");
            };
    } >> [onError](const DrogonDbException& e) {
        spdlog::error("listUsers count DB error: {}", e.base().what());
        onError(k500InternalServerError, "Internal server error");
    };
}

} // namespace services
