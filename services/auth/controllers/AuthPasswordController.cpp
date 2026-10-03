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
#include "app_login_redirect.h"

#include <drogon/HttpResponse.h>
#include <string>

namespace controllers
{

void AuthPasswordController::forgotPassword(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(appLoginRedirect());
}

void AuthPasswordController::resetPassword(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& /*token*/)
{
    cb(appLoginRedirect());
}

void AuthPasswordController::confirmEmail(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb,
    const std::string& /*token*/)
{
    cb(appLoginRedirect());
}

} // namespace controllers
