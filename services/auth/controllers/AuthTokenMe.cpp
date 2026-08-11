/**
 * @file AuthTokenMe.cpp
 * @brief "me" endpoint for AuthTokenController.
 */

#include "AuthTokenController.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace controllers
{

void AuthTokenController::me(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    auto userId =
        req->attributes()->get<std::string>(
            "user_id");
    json user = {
        {"id", userId},
        {"email", "user@example.com"},
        {"role", req->attributes()
            ->get<std::string>("user_role")}};
    cb(::utils::jsonOk(user));
}

} // namespace controllers
