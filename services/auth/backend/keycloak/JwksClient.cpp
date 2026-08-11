/**
 * @file JwksClient.cpp
 * @brief JWKS fetch + cache implementation.
 */

#include "JwksClient.h"
#include "jwks_client_helpers.h"

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace services::auth::keycloak
{

static constexpr std::chrono::minutes kTtl{10};

JwksClient::JwksClient(std::string jwksUrl)
    : jwksUrl_(std::move(jwksUrl))
{
}

bool JwksClient::stale_() const
{
    if (keys_.empty()) return true;
    return std::chrono::steady_clock::now() - fetchedAt_
           > kTtl;
}

void JwksClient::doFetch_()
{
    const auto [base, path] = detail::splitUrl(jwksUrl_);
    auto client = drogon::HttpClient::newHttpClient(base);
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Get);
    req->setPath(path);
    req->addHeader("Accept", "application/json");
    auto [rc, resp] = client->sendRequest(req, 5.0);
    if (rc != drogon::ReqResult::Ok || !resp) {
        spdlog::warn("JWKS fetch failed: {}", jwksUrl_);
        return;
    }
    auto j = nlohmann::json::parse(
        std::string(resp->body()), nullptr, false);
    if (j.is_discarded() || !j.contains("keys")) {
        spdlog::warn("JWKS body invalid");
        return;
    }
    std::unordered_map<std::string, std::string> next;
    for (const auto& k : j["keys"]) {
        if (!k.contains("kid") || !k.contains("n")
            || !k.contains("e"))
            continue;
        if (k.value("kty", "") != "RSA") continue;
        auto pem = detail::rsaPemFromJwk(
            k["n"].get<std::string>(),
            k["e"].get<std::string>());
        if (!pem.empty())
            next[k["kid"].get<std::string>()] =
                std::move(pem);
    }
    if (!next.empty()) {
        keys_ = std::move(next);
        fetchedAt_ = std::chrono::steady_clock::now();
    }
}

std::optional<std::string>
JwksClient::getPublicKeyPem(const std::string& kid)
{
    std::lock_guard lk(mu_);
    if (stale_()) doFetch_();
    auto it = keys_.find(kid);
    if (it != keys_.end()) return it->second;
    // kid miss: force refresh once
    doFetch_();
    it = keys_.find(kid);
    if (it != keys_.end()) return it->second;
    return std::nullopt;
}

void JwksClient::refresh()
{
    std::lock_guard lk(mu_);
    doFetch_();
}

void JwksClient::injectForTest(const std::string& kid,
                               const std::string& pem)
{
    std::lock_guard lk(mu_);
    keys_[kid] = pem;
    fetchedAt_ = std::chrono::steady_clock::now();
}

} // namespace services::auth::keycloak
