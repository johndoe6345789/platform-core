/**
 * @file auth_client.cpp
 * @brief Internal HTTP client to the auth service validator.
 */
#include "http-filters/backend/auth_client.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <format>

namespace filters
{

namespace
{
std::string envOr(const char* k, const char* d)
{
    const char* v = std::getenv(k);
    return (v && *v) ? std::string(v) : std::string(d);
}

drogon::HttpClientPtr authClient()
{
    static auto c = drogon::HttpClient::newHttpClient(
        std::format("http://{}:{}",
                    envOr("AUTH_HOST", "nextra-auth"),
                    envOr("AUTH_PORT", "9001")),
        drogon::app().getLoop());
    return c;
}
} // namespace

void validateViaAuthService(
    const drogon::HttpRequestPtr& req,
    std::function<void(const std::string&,
                       const std::string&)> onOk,
    std::function<void(
        const drogon::HttpResponsePtr&)> onFail)
{
    auto out = drogon::HttpRequest::newHttpRequest();
    out->setMethod(drogon::Get);
    out->setPath("/api/auth/validate");
    if (auto a = req->getHeader("Authorization"); !a.empty())
        out->addHeader("Authorization", a);
    if (auto ck = req->getHeader("Cookie"); !ck.empty())
        out->addHeader("Cookie", ck);

    authClient()->sendRequest(
        out,
        [onOk = std::move(onOk),
         onFail](drogon::ReqResult r,
                 const drogon::HttpResponsePtr& resp) {
            if (r != drogon::ReqResult::Ok) {
                spdlog::warn("auth service unreachable");
                onFail(::utils::jsonError(
                    drogon::k503ServiceUnavailable,
                    "Auth service unavailable",
                    "AUTH_009"));
                return;
            }
            if (resp->getStatusCode() != drogon::k200OK) {
                onFail(::utils::jsonError(
                    drogon::k401Unauthorized,
                    "Invalid or expired credential",
                    "AUTH_005"));
                return;
            }
            std::string role = resp->getHeader("X-User-Roles");
            if (auto c = role.find(','); c != std::string::npos)
                role = role.substr(0, c);
            if (role.empty())
                role = "user";
            onOk(resp->getHeader("X-User-Id"), role);
        });
}

} // namespace filters
