/**
 * @file ApiKeyCrud.cpp
 * @brief User API key set and get operations.
 */

#include "api-keys/backend/ApiKeyService.h"
#include "api-keys/backend/api_key_crud_queries.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void ApiKeyService::setUserKey(
    const std::string& userId,
    const std::string& provider,
    const std::string& apiKey,
    const std::string& model,
    Callback onOk, ErrCallback onErr)
{
    *db() << std::string(kSetUserKeySql)
          << userId << provider
          << apiKey << model
        >> [onOk](const Result& r) {
            const auto& row = r[0];
            onOk({{"provider",
                   row["provider"]
                       .as<std::string>()},
                  {"model",
                   row["model"]
                       .as<std::string>()},
                  {"saved", true}});
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error("setUserKey: {}",
                          e.base().what());
            onErr(k500InternalServerError,
                  "Failed to save API key");
        };
}

void ApiKeyService::getUserKey(
    const std::string& userId,
    const std::string& provider,
    Callback onOk, ErrCallback /*onErr*/)
{
    *db() << std::string(kGetUserKeySql)
          << userId << provider
        >> [onOk](const Result& r) {
            if (r.empty()) {
                onOk({{"configured", false}});
                return;
            }
            const auto& row = r[0];
            onOk({{"configured", true},
                  {"provider",
                   row["provider"]
                       .as<std::string>()},
                  {"model",
                   row["model"]
                       .as<std::string>()},
                  {"maskedKey",
                   mask(row["api_key"]
                            .as<std::string>())}});
        }
        >> [onOk](const DrogonDbException& e) {
            spdlog::error("getUserKey: {}",
                          e.base().what());
            onOk({{"configured", false}});
        };
}

} // namespace services
