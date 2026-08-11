/**
 * @file ProviderConfigLoader.cpp
 * @brief JSON + env backed ProviderConfig loader.
 */

#include "ProviderConfigLoader.h"

#include <cstdlib>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>

namespace services::auth::oauth
{

namespace
{

nlohmann::json loadRoot()
{
    std::ifstream f("constants/oauth.json");
    if (!f) {
        return nlohmann::json::object();
    }
    nlohmann::json j;
    f >> j;
    return j;
}

std::string envOr(const std::string& key,
                  const std::string& fallback)
{
    const char* v = std::getenv(key.c_str());
    return v ? std::string(v) : fallback;
}

} // namespace

std::optional<ProviderConfig>
loadProviderConfig(const std::string& provider)
{
    auto root = loadRoot();
    if (!root.contains("providers") ||
        !root["providers"].contains(provider)) {
        return std::nullopt;
    }
    const auto& p = root["providers"][provider];
    ProviderConfig cfg;
    cfg.name = provider;
    cfg.authorizeUrl =
        p.value("authorizeUrl", std::string{});
    cfg.tokenUrl = p.value("tokenUrl", std::string{});
    cfg.userInfoUrl =
        p.value("userInfoUrl", std::string{});
    cfg.scope = p.value("scope", std::string{});
    auto idKey = p.value("clientIdEnv", std::string{});
    auto secKey = p.value("clientSecretEnv", std::string{});
    cfg.clientId = envOr(idKey, "");
    cfg.clientSecret = envOr(secKey, "");
    return cfg;
}

bool providerWantsJsonAccept(const std::string& provider)
{
    auto root = loadRoot();
    if (!root.contains("providers") ||
        !root["providers"].contains(provider)) {
        return false;
    }
    return root["providers"][provider]
        .value("acceptJson", false);
}

std::string defaultPortalOrigin()
{
    auto root = loadRoot();
    return root.value(
        "defaultPortalOrigin",
        std::string{"http://localhost:8889"});
}

} // namespace services::auth::oauth
