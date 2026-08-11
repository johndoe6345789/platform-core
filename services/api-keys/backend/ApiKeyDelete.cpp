/**
 * @file ApiKeyDelete.cpp
 * @brief User API key deletion operation.
 */

#include "api-keys/backend/ApiKeyService.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void ApiKeyService::deleteUserKey(
    const std::string& userId,
    const std::string& provider,
    Callback onOk, ErrCallback onErr)
{
    const std::string sql = R"(
        DELETE FROM user_api_keys
        WHERE user_id = $1 AND provider = $2
    )";

    *db() << sql << userId << provider
        >> [onOk](const Result&) {
            onOk({{"deleted", true}});
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error("deleteUserKey: {}",
                          e.base().what());
            onErr(k500InternalServerError,
                  "Failed to delete key");
        };
}

} // namespace services
