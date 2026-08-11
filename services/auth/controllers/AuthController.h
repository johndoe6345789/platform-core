#pragma once
/**
 * @file AuthController.h
 * @brief Core authentication endpoints: register and login.
 *
 * Token management (logout, refresh, me) is in
 * AuthTokenController.h.  Password recovery and email
 * confirmation are in AuthPasswordController.h.
 */

#include <drogon/HttpController.h>
#include <string>

namespace controllers
{

class AuthController
    : public drogon::HttpController<AuthController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::registerUser,
                  "/api/auth/register", drogon::Post);
    ADD_METHOD_TO(AuthController::login,
                  "/api/auth/login", drogon::Post);
    METHOD_LIST_END

    /** @brief Register a new user account. */
    void registerUser(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Authenticate and issue tokens. */
    void login(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
