/** @brief Cookie-based auth filter — self-contained, no auth dep. */
#include "CookieAuthFilter.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "drogon-host/backend/utils/JwtUtil.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

namespace filters
{

namespace
{
constexpr std::string_view kBearer = "Bearer ";

void checkBlocklist(
    const std::string& jti,
    std::function<void(bool)> cb)
{
    auto db = drogon::app().getDbClient();
    *db << "SELECT 1 FROM token_blocklist "
           "WHERE jti=$1 LIMIT 1"
        << jti
        >> [cb](const drogon::orm::Result& r) {
            cb(!r.empty());
        }
        >> [cb](const drogon::orm::DrogonDbException& e) {
            spdlog::error("blocklist check: {}",
                e.base().what());
            cb(true); // fail closed on DB error
        };
}
} // anonymous namespace

void CookieAuthFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& cb,
    drogon::FilterChainCallback&& ccb)
{
    auto token = req->getCookie("nextra_sso");
    if (token.empty()) {
        auto auth =
            req->getHeader("Authorization");
        if (auth.size() > kBearer.size()
            && auth.substr(0, kBearer.size())
                   == kBearer) {
            token = auth.substr(kBearer.size());
        }
    }

    if (token.empty()) {
        cb(::utils::jsonError(
            drogon::k401Unauthorized,
            "Authentication required",
            "AUTH_006"));
        return;
    }

    utils::JwtClaims claims;
    try {
        claims = ::utils::verifyToken(token);
    } catch (...) {
        cb(::utils::jsonError(
            drogon::k401Unauthorized,
            "Invalid token", "AUTH_005"));
        return;
    }

    if (claims.isRefresh) {
        cb(::utils::jsonError(
            drogon::k401Unauthorized,
            "Refresh token not allowed",
            "AUTH_007"));
        return;
    }

    const auto userId = claims.userId;
    const auto role   = claims.role;
    checkBlocklist(
        token,
        [req, cb, ccb = std::move(ccb),
         userId, role](bool blocked) mutable {
            if (blocked) {
                cb(::utils::jsonError(
                    drogon::k401Unauthorized,
                    "Token revoked", "AUTH_004"));
                return;
            }
            req->attributes()->insert(
                "user_id", userId);
            req->attributes()->insert(
                "user_role", role);
            ccb();
        });
}

} // namespace filters
