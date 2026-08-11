/**
 * @file AdminEnvController.cpp
 * @brief Returns allowlisted env vars (masked).
 */

#include "AdminEnvController.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "admin_env_config.h"

#include <cstdlib>
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AdminEnvController::getEnvVars(
    const drogon::HttpRequestPtr& req,
    Cb&& cb)
{
    auto role = req->attributes()
        ->get<std::string>("user_role");
    if (role != "admin") {
        cb(::utils::jsonError(
            drogon::k403Forbidden,
            "Admin access required"));
        return;
    }

    json vars = json::array();
    for (const auto& [name, group] : kVars) {
        const char* v = std::getenv(name);
        std::string val = v ? v : "";
        vars.push_back(
            {{"name", name},
             {"group", group},
             {"value",
              isSensitive(name)
                  ? maskValue(val)
                  : val},
             {"set", !val.empty()}});
    }
    cb(::utils::jsonOk(
        {{"vars", vars},
         {"source", "backend"}}));
}

} // namespace controllers
