#pragma once
/**
 * @file password_reset_execute.h
 * @brief Execute a password reset with a valid token.
 */

#include "auth_service_types.h"

#include <string>

namespace services
{

/**
 * @class PasswordResetExecute
 * @brief Validates the reset token and updates the password.
 */
class PasswordResetExecute
{
  public:
    PasswordResetExecute() = default;
    ~PasswordResetExecute() = default;

    /**
     * @brief Reset password using a valid reset token.
     * @param token       One-time reset token.
     * @param newPassword New plain-text password.
     * @param onSuccess   Callback with `{reset: true}`.
     * @param onError     Callback on invalid/expired token.
     */
    void execute(
        const std::string& token,
        const std::string& newPassword,
        Callback onSuccess,
        ErrCallback onError);
};

} // namespace services
