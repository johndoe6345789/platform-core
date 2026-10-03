/**
 * @file AuthSessionRegister.cpp
 * @brief POST /api/auth/session — registers a Keycloak JTI
 *        so subsequent JwtAuthFilter session checks pass.
 *        No JwtAuthFilter here — we validate manually since
 *        the session does not yet exist in the store.
 */
#include "AuthTokenController.h"
#include "auth/backend/keycloak/KeycloakVerifier.h"
#include "auth/backend/SessionStore.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#define JWT_DISABLE_PICOJSON
#include <jwt-cpp/traits/nlohmann-json/defaults.h>
#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <ctime>
#include <string>
#include <string_view>

namespace controllers
{
using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;

namespace {
constexpr std::string_view kBearer = "Bearer ";
std::string toIso8601(
    const std::chrono::system_clock::time_point& tp)
{
    auto tt = std::chrono::system_clock::to_time_t(tp);
    char buf[32] = {};
    std::strftime(buf, sizeof(buf),
                  "%Y-%m-%dT%H:%M:%SZ",
                  std::gmtime(&tt));
    return buf;
}
} // anonymous namespace

void AuthTokenController::registerSession(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    const auto auth = req->getHeader("Authorization");
    if (auth.size() < kBearer.size()
        || auth.substr(0, kBearer.size()) != kBearer) {
        cb(::utils::jsonError(drogon::k401Unauthorized,
            "Missing Bearer token", "AUTH_006"));
        return;
    }
    const auto token = auth.substr(kBearer.size());

    auto kc =
        services::auth::keycloak::defaultVerifier()
            .verify(token);
    if (!kc) {
        cb(::utils::jsonError(drogon::k401Unauthorized,
            "Invalid or expired token", "AUTH_005"));
        return;
    }

    std::string jti, expStr;
    try {
        auto d = jwt::decode(token);
        if (d.has_id()) jti = d.get_id();
        if (d.has_expires_at())
            expStr = toIso8601(d.get_expires_at());
    } catch (...) {}

    if (jti.empty()) {
        cb(::utils::jsonOk({{"registered", false}}));
        return;
    }

    const auto ua = req->getHeader("User-Agent");
    const auto ip = req->peerAddr().toIp();

    services::auth::SessionStore::createSession(
        jti, kc->sub, expStr, ua, ip,
        [cb = std::move(cb)](bool) {
            cb(::utils::jsonOk(
                {{"registered", true}}));
        },
        [cb = std::move(cb)](const std::string& e) {
            spdlog::debug("registerSession: {}", e);
            cb(::utils::jsonOk(
                {{"registered", true}}));
        });
}

} // namespace controllers
