/**
 * @file ApiKeyService.cpp
 * @brief API key resolution chain: user -> system -> env.
 */

#include "api-keys/backend/ApiKeyService.h"
#include "api-keys/backend/ApiKeyFallback.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto ApiKeyService::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

auto ApiKeyService::mask(
    const std::string& key) -> std::string
{
    if (key.size() <= 8) return "****";
    return key.substr(0, 4) + "..."
           + key.substr(key.size() - 4);
}

void ApiKeyService::resolve(
    const std::string& userId,
    AiProvider provider,
    Callback onOk,
    ErrCallback /*onErr*/)
{
    const std::string prov =
        (provider == AiProvider::OPENAI)
            ? "openai" : "claude";

    const std::string sql = R"(
        SELECT api_key, model
        FROM user_api_keys
        WHERE user_id = $1 AND provider = $2
    )";

    *db() << sql << userId << prov
        >> [prov, onOk](const Result& r) {
            if (!r.empty()) {
                onOk({{"apiKey",
                       r[0]["api_key"]
                           .as<std::string>()},
                      {"model",
                       r[0]["model"]
                           .as<std::string>()},
                      {"source", "user"}});
                return;
            }
            fallbackSystem(prov, onOk);
        }
        >> [prov, onOk](
               const DrogonDbException& e) {
            spdlog::warn("user key lookup: {}",
                         e.base().what());
            fallbackSystem(prov, onOk);
        };
}

} // namespace services
