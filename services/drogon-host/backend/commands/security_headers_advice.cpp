/**
 * @file security_headers_advice.cpp
 * @brief Loads security-headers.json and registers a
 *        Drogon pre-sending advice that stamps protective
 *        HTTP response headers on every outgoing response.
 */

#include "security_headers_advice.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <map>
#include <string>

namespace commands
{

namespace
{

/// @brief Load header map from constants JSON.
auto loadHeaders() -> std::map<std::string, std::string>
{
    std::map<std::string, std::string> out;
    constexpr auto kPath =
        "constants/security-headers.json";
    std::ifstream fs{kPath};
    if (!fs.is_open()) {
        spdlog::warn(
            "security_headers_advice: {} not found,"
            " security headers disabled",
            kPath);
        return out;
    }
    const auto j = nlohmann::json::parse(fs);
    for (auto it = j.begin(); it != j.end(); ++it) {
        if (it.value().is_string()) {
            out.emplace(
                it.key(),
                it.value().get<std::string>());
        }
    }
    spdlog::debug(
        "security_headers_advice: loaded {} headers",
        out.size());
    return out;
}

} // anonymous namespace

void registerSecurityHeadersAdvice()
{
    auto headers = loadHeaders();
    if (headers.empty()) {
        return;
    }
    drogon::app().registerPreSendingAdvice(
        [hdrs = std::move(headers)](
            const drogon::HttpRequestPtr& /*req*/,
            const drogon::HttpResponsePtr& resp) {
            for (const auto& [name, value] : hdrs) {
                resp->addHeader(name, value);
            }
        });
}

} // namespace commands
