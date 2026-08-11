#pragma once
/**
 * @file feature_toggle_list.h
 * @brief Row-to-JSON helper for feature toggle list.
 */

#include <drogon/orm/Result.h>
#include <nlohmann/json.hpp>

namespace controllers
{

using json = nlohmann::json;

/**
 * @brief Convert feature toggle rows to JSON array.
 * @param r  DB query result.
 * @return JSON array of feature toggle objects.
 */
inline auto featureRowsToJson(
    const drogon::orm::Result& r) -> json
{
    json arr = json::array();
    for (const auto& row : r) {
        arr.push_back({
            {"key",
             row["key"].as<std::string>()},
            {"enabled",
             row["enabled"].as<bool>()},
            {"description",
             row["description"]
                 .as<std::string>()},
            {"scope",
             row["scope"].as<std::string>()},
        });
    }
    return arr;
}

} // namespace controllers
