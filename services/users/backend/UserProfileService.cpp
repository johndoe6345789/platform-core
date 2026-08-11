/**
 * @file UserProfileService.cpp
 * @brief Implementation of user profile update operations.
 */

#include "users/backend/UserProfileService.h"
#include "users/backend/UserLookupService.h"

#include <drogon/drogon.h>
#include <drogon/orm/DbClient.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto UserProfileService::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

void UserProfileService::updateUser(const std::string& id, const json& fields,
                                    Callback onSuccess, ErrCallback onError)
{
    std::vector<std::string> setClauses;
    std::vector<std::string> values;
    int paramIdx = 1;

    for (const auto& col : kEditableFields) {
        if (fields.contains(col) && fields[col].is_string()) {
            setClauses.push_back(fmt::format("{} = ${}", col, ++paramIdx));
            values.push_back(fields[col].get<std::string>());
        }
    }

    if (setClauses.empty()) {
        onError(k400BadRequest, "No valid fields to update");
        return;
    }

    setClauses.emplace_back("updated_at = NOW()");

    std::string setStr;
    for (std::size_t i = 0; i < setClauses.size(); ++i) {
        if (i > 0)
            setStr += ", ";
        setStr += setClauses[i];
    }

    auto sql = fmt::format(R"(
        UPDATE users SET {}
        WHERE id = $1
        RETURNING id, email, username, display_name,
                  role, email_confirmed, total_points,
                  current_streak, avatar_url, bio,
                  created_at
    )",
                           setStr);

    auto dbClient = db();
    auto binder = *dbClient << sql;
    binder << id;
    for (const auto& v : values) {
        binder << v;
    }
    binder >> [onSuccess, onError](const Result& r) {
        if (r.empty()) {
            onError(k404NotFound, "User not found");
            return;
        }
        onSuccess(UserLookupService::rowToJson(r[0]));
    };
    binder >> [onError](const DrogonDbException& e) {
        spdlog::error("updateUser DB error: {}", e.base().what());
        onError(k500InternalServerError, "Internal server error");
    };
    binder.exec();
}

} // namespace services
