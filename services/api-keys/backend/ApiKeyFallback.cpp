/**
 * @file ApiKeyFallback.cpp
 * @brief Fallback key resolution: system settings and
 *        environment variables.
 */

#include "api-keys/backend/ApiKeyFallback.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <cstdlib>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void fallbackEnv(
    const std::string& prov,
    Callback onOk)
{
    const char* envKey =
        (prov == "openai")
            ? "OPENAI_API_KEY"
            : "ANTHROPIC_API_KEY";
    const char* v = std::getenv(envKey);
    std::string key = v ? v : "";
    onOk({{"apiKey", key},
          {"model", ""},
          {"source", key.empty()
                         ? "none" : "env"}});
}

void fallbackSystem(
    const std::string& prov,
    Callback onOk)
{
    const std::string keyName =
        prov + "_api_key";

    auto dbC = drogon::app().getDbClient();
    const std::string sql = R"(
        SELECT value FROM system_settings
        WHERE key = $1
    )";

    *dbC << sql << keyName
        >> [prov, onOk](const Result& r) {
            if (!r.empty()) {
                auto val = r[0]["value"]
                    .as<std::string>();
                if (!val.empty()) {
                    onOk({{"apiKey", val},
                          {"model", ""},
                          {"source", "system"}});
                    return;
                }
            }
            fallbackEnv(prov, onOk);
        }
        >> [prov, onOk](
               const DrogonDbException& e) {
            spdlog::warn(
                "system key lookup: {}",
                e.base().what());
            fallbackEnv(prov, onOk);
        };
}

} // namespace services
