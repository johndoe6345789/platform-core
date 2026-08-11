#pragma once
/**
 * @file AuthPasswordController.h
 * @brief Password recovery and email confirmation endpoints:
 *        forgot-password, reset-password, confirm-email.
 */

#include <drogon/HttpController.h>
#include <string>

namespace controllers
{

class AuthPasswordController
    : public drogon::HttpController<AuthPasswordController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthPasswordController::forgotPassword,
                  "/api/auth/forgot-password", drogon::Post);
    ADD_METHOD_TO(AuthPasswordController::resetPassword,
                  "/api/auth/reset-password/{token}",
                  drogon::Post);
    ADD_METHOD_TO(AuthPasswordController::confirmEmail,
                  "/api/auth/confirm/{token}", drogon::Get);
    METHOD_LIST_END

    /** @brief Send a password-reset email. */
    void forgotPassword(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Reset password using a one-time token. */
    void resetPassword(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        const std::string& token);

    /** @brief Confirm a user's email address. */
    void confirmEmail(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        const std::string& token);
};

} // namespace controllers
