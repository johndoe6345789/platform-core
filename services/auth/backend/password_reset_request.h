#pragma once
/**
 * @file password_reset_request.h
 * @brief Request a password-reset email.
 */

#include "auth_service_types.h"

#include <string>

namespace services
{

/**
 * @class PasswordResetRequest
 * @brief Generates a reset token and sends the reset email.
 */
class PasswordResetRequest
{
  public:
    PasswordResetRequest() = default;
    ~PasswordResetRequest() = default;

    /**
     * @brief Request a password-reset email.
     *
     * Always returns success to avoid leaking whether
     * the email exists.
     *
     * @param email     Email address to send the reset to.
     * @param onSuccess Callback (always fires).
     * @param onError   Callback on internal error only.
     */
    void execute(
        const std::string& email,
        Callback onSuccess,
        ErrCallback onError);
};

} // namespace services
