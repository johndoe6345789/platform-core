/**
 * @file SessionFilter.cpp
 * @brief JWT session validation filter.
 */

#include "SessionFilter.h"
#include "../services/Globals.h"
#include "../services/SessionService.h"

using namespace drogon;

namespace pgadmin
{

void SessionFilter::doFilter(
    const HttpRequestPtr& req,
    FilterCallback&& cb,
    FilterChainCallback&& ccb)
{
    auto auth = req->getHeader("Authorization");
    std::string token;

    if (auth.starts_with("Bearer ")) {
        token = auth.substr(7);
    } else {
        // Also check cookie fallback.
        token = req->getCookie("pgadmin-session");
    }

    if (token.empty()) {
        auto r = HttpResponse::newHttpJsonResponse(
            Json::Value("Unauthorized"));
        r->setStatusCode(k401Unauthorized);
        cb(r);
        return;
    }

    auto payload = SessionService::verify(
        token, Globals::jwtSecret);
    if (!payload) {
        auto r = HttpResponse::newHttpJsonResponse(
            Json::Value("Invalid session"));
        r->setStatusCode(k401Unauthorized);
        cb(r);
        return;
    }

    req->attributes()->insert(
        "username", payload->username);
    ccb();
}

} // namespace pgadmin
