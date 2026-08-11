/**
 * @file TokenExchange.cpp
 * @brief Drogon HttpClient-backed OAuth token exchange.
 */

#include "TokenExchange.h"
#include "HttpHelpers.h"

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <string>

namespace services::auth::oauth
{

namespace
{

std::string buildBody(
    const ProviderConfig& cfg,
    const std::string& code,
    const std::string& verifier,
    const std::string& redirectUri)
{
    std::string b = "grant_type=authorization_code";
    b += "&code=" + urlEncode(code);
    b += "&redirect_uri=" + urlEncode(redirectUri);
    b += "&client_id=" + urlEncode(cfg.clientId);
    b += "&client_secret=" + urlEncode(cfg.clientSecret);
    b += "&code_verifier=" + urlEncode(verifier);
    return b;
}

TokenResponse parseTokenJson(const std::string& body)
{
    TokenResponse t;
    auto j = nlohmann::json::parse(body, nullptr, false);
    if (j.is_discarded() || !j.is_object()) return t;
    if (j.contains("access_token"))
        t.accessToken = j["access_token"].get<std::string>();
    if (j.contains("id_token"))
        t.idToken = j["id_token"].get<std::string>();
    if (j.contains("refresh_token"))
        t.refreshToken =
            j["refresh_token"].get<std::string>();
    if (j.contains("token_type"))
        t.tokenType = j["token_type"].get<std::string>();
    if (j.contains("expires_in") &&
        j["expires_in"].is_number_integer())
        t.expiresIn = j["expires_in"].get<int>();
    return t;
}

} // namespace

TokenResponse postTokenRequest(
    const ProviderConfig& cfg,
    const std::string& code,
    const std::string& codeVerifier,
    const std::string& redirectUri,
    bool acceptJson)
{
    if (cfg.tokenUrl.empty()) return {};
    const auto [base, path] = splitUrl(cfg.tokenUrl);
    auto client = drogon::HttpClient::newHttpClient(base);
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath(path);
    req->setContentTypeString(
        "application/x-www-form-urlencoded");
    if (acceptJson)
        req->addHeader("Accept", "application/json");
    req->setBody(
        buildBody(cfg, code, codeVerifier, redirectUri));
    auto [rc, resp] = client->sendRequest(req, 10.0);
    if (rc != drogon::ReqResult::Ok || !resp) {
        spdlog::warn("oauth token POST transport fail");
        return {};
    }
    return parseTokenJson(std::string(resp->body()));
}

} // namespace services::auth::oauth
