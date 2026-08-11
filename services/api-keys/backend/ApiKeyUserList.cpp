/**
 * @file ApiKeyUserList.cpp
 * @brief Bulk user key listing operation.
 */

#include "api-keys/backend/ApiKeyService.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void ApiKeyService::getAllUserKeys(
    const std::string& userId,
    Callback onOk, ErrCallback /*onErr*/)
{
    const std::string sql = R"(
        SELECT provider, api_key, model
        FROM user_api_keys
        WHERE user_id = $1
        ORDER BY provider
    )";

    *db() << sql << userId
        >> [onOk](const Result& r) {
            json keys = json::array();
            for (const auto& row : r) {
                keys.push_back(
                    {{"provider",
                      row["provider"]
                          .as<std::string>()},
                     {"model",
                      row["model"]
                          .as<std::string>()},
                     {"maskedKey",
                      ApiKeyService::mask(
                          row["api_key"]
                              .as<std::string>()
                      )}});
            }
            onOk({{"keys", keys}});
        }
        >> [onOk](const DrogonDbException& e) {
            spdlog::error(
                "getAllUserKeys: {}",
                e.base().what());
            onOk({{"keys", json::array()}});
        };
}

} // namespace services
