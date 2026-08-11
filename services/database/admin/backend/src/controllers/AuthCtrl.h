/**
 * @file AuthCtrl.h
 * @brief Login and logout endpoints.
 */

#pragma once

#include <drogon/HttpController.h>

namespace pgadmin
{

/// @brief POST /api/auth/login, /api/auth/logout.
class AuthCtrl
    : public drogon::HttpController<AuthCtrl>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthCtrl::login,
                  "/api/auth/login", drogon::Post);
    ADD_METHOD_TO(AuthCtrl::logout,
                  "/api/auth/logout", drogon::Post);
    METHOD_LIST_END

    /// @brief Authenticate with username/password.
    void login(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);

    /// @brief Clear session.
    void logout(
        const drogon::HttpRequestPtr&,
        std::function<void(
            const drogon::HttpResponsePtr&)>&&);
};

} // namespace pgadmin
