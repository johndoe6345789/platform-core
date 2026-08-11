/**
 * @file AuthPasswordController.cpp
 * @brief DEPRECATED: Keycloak migration phase 3.
 *
 * Password recovery and email confirmation endpoints are
 * now owned by Keycloak. The function bodies below have
 * been replaced with 302 redirects so existing clients and
 * tests continue to receive a 3xx. The files are retained
 * per template-repo policy (every character is a feature).
 */

#include "AuthPasswordController.h"

#include <drogon/HttpResponse.h>
#include <string>

namespace controllers
{

namespace
{
constexpr const char* kKcUpdatePassword =
    "http://localhost:8889/sso/realms/nextra"
    "/protocol/openid-connect/auth"
    "?client_id=nextra-app&response_type=code"
    "&scope=openid"
    "&redirect_uri=http%3A%2F%2Flocalhost%3A8889"
    "%2Fapp%2Fen%2Fauth%2Fcallback"
    "&kc_action=UPDATE_PASSWORD";

constexpr const char* kKcAccount =
    "http://localhost:8889/sso/realms/nextra/account/";

drogon::HttpResponsePtr redirect(const char* url)
{
    return drogon::HttpResponse::newRedirectionResponse(
        url, drogon::k302Found);
}
} // namespace

void AuthPasswordController::forgotPassword(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(redirect(kKcUpdatePassword));
}

void AuthPasswordController::resetPassword(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& /*token*/)
{
    cb(redirect(kKcUpdatePassword));
}

void AuthPasswordController::confirmEmail(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& /*token*/)
{
    cb(redirect(kKcAccount));
}

} // namespace controllers
