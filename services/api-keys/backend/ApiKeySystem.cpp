/**
 * @file ApiKeySystem.cpp
 * @brief System settings get/set for API keys.
 */

#include "api-keys/backend/ApiKeyService.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void ApiKeyService::getSystemSetting(
    const std::string& key,
    Callback onOk, ErrCallback /*onErr*/)
{
    const std::string sql = R"(
        SELECT value FROM system_settings
        WHERE key = $1
    )";

    *db() << sql << key
        >> [key, onOk](const Result& r) {
            if (r.empty()) {
                onOk({{"key", key},
                      {"value", ""},
                      {"configured", false}});
                return;
            }
            onOk({{"key", key},
                  {"value",
                   r[0]["value"]
                       .as<std::string>()},
                  {"configured", true}});
        }
        >> [key, onOk](
               const DrogonDbException& e) {
            spdlog::error(
                "getSystemSetting: {}",
                e.base().what());
            onOk({{"key", key},
                  {"value", ""},
                  {"configured", false}});
        };
}

void ApiKeyService::setSystemSetting(
    const std::string& key,
    const std::string& value,
    Callback onOk, ErrCallback onErr)
{
    const std::string sql = R"(
        INSERT INTO system_settings (key, value)
        VALUES ($1, $2)
        ON CONFLICT (key)
        DO UPDATE SET value = $2,
            updated_at = NOW()
        RETURNING key
    )";

    *db() << sql << key << value
        >> [key, onOk](const Result&) {
            onOk({{"key", key},
                  {"saved", true}});
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error(
                "setSystemSetting: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Failed to save setting");
        };
}

} // namespace services
