#pragma once
/**
 * @file AuthService.h
 * @brief Thin facade that delegates to focused
 *        authentication sub-services.
 */

#include "auth_service_types.h"
#include "email/backend/email_confirm_service.h"
#include "password_reset_execute.h"
#include "password_reset_request.h"
#include "registration_service.h"
#include "session_service.h"
#include "token_refresh_service.h"
#include "token_service.h"
#include <functional>
#include <string>

namespace services
{

/**
 * @class AuthService
 * @brief Facade delegating to sub-services.
 */
class AuthService
{
  public:
    AuthService() = default;
    ~AuthService() = default;

    /** @brief Register a new user. */
    void registerUser(
        const std::string& email,
        const std::string& username,
        const std::string& password,
        const std::string& displayName,
        Callback onOk, ErrCallback onErr);

    /** @brief Login via email + password. */
    void loginUser(
        const std::string& email,
        const std::string& password,
        Callback onOk, ErrCallback onErr);

    /** @brief Refresh an expired access token. */
    void refreshAccessToken(
        const std::string& refreshToken,
        Callback onOk, ErrCallback onErr);

    /** @brief Block a token JTI (logout). */
    void logoutUser(
        const std::string& jti,
        Callback onOk, ErrCallback onErr);

    /** @brief Confirm email via one-time token. */
    void confirmEmail(
        const std::string& token,
        Callback onOk, ErrCallback onErr);

    /** @brief Request a password-reset email. */
    void requestPasswordReset(
        const std::string& email,
        Callback onOk, ErrCallback onErr);

    /** @brief Execute a password reset. */
    void resetPassword(
        const std::string& token,
        const std::string& newPassword,
        Callback onOk, ErrCallback onErr);

    /** @brief Check if a JTI is blocklisted. */
    void isTokenBlocked(
        const std::string& jti,
        std::function<void(bool)> cb);

  private:
    RegistrationService registration_;
    SessionService session_;
    TokenRefreshService tokenRefresh_;
    TokenService token_;
    EmailConfirmService emailConfirm_;
    PasswordResetRequest pwResetReq_;
    PasswordResetExecute pwResetExec_;
};

} // namespace services
