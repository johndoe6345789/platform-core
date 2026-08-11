/**
 * @file UserInfoFetch.cpp
 * @brief Provider userinfo HTTP GET implementation.
 */

#include "UserInfoFetch.h"
#include "HttpHelpers.h"

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <spdlog/spdlog.h>

#include <string>

namespace services::auth::oauth
{

nlohmann::json fetchUserInfo(
    const ProviderConfig& cfg,
    const std::string& accessToken)
{
    if (cfg.userInfoUrl.empty()) return nullptr;
    const auto [base, path] = splitUrl(cfg.userInfoUrl);
    auto client = drogon::HttpClient::newHttpClient(base);
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Get);
    req->setPath(path);
    req->addHeader("Accept", "application/json");
    req->addHeader("Authorization",
                   "Bearer " + accessToken);
    req->addHeader("User-Agent", "nextra-oauth/1.0");
    auto [rc, resp] = client->sendRequest(req, 10.0);
    if (rc != drogon::ReqResult::Ok || !resp) {
        spdlog::warn("oauth userinfo GET fail");
        return nullptr;
    }
    auto j = nlohmann::json::parse(
        std::string(resp->body()), nullptr, false);
    if (j.is_discarded()) return nullptr;
    return j;
}

} // namespace services::auth::oauth
