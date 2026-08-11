#pragma once
/**
 * @file status_controller_helpers.h
 * @brief Shared helpers for StatusController implementation
 *        files. Kept header-only to preserve the 100-LOC cap.
 */

#include <drogon/orm/Result.h>
#include <nlohmann/json.hpp>
#include <string>

namespace nextra::controllers::status_helpers
{

/**
 * @brief Convert a drogon ORM row to the public JSON shape.
 * @param row Result row pointing at an incidents record.
 * @return nlohmann::json object ready for the response array.
 */
inline nlohmann::json rowToJson(
    const drogon::orm::Row& row)
{
    nlohmann::json j;
    j["id"] = row["id"].as<std::int64_t>();
    j["title"] = row["title"].as<std::string>();
    j["body"] = row["body"].as<std::string>();
    j["severity"] = row["severity"].as<std::string>();
    j["status"] = row["status"].as<std::string>();
    j["started_at"] =
        row["started_at"].as<std::string>();
    if (!row["resolved_at"].isNull()) {
        j["resolved_at"] =
            row["resolved_at"].as<std::string>();
    }
    return j;
}

} // namespace nextra::controllers::status_helpers
