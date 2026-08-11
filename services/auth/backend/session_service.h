#pragma once
/**
 * @file session_service.h
 * @brief User login (credential verification).
 */

#include "auth_service_types.h"

#include <string>

namespace services
{

/**
 * @class SessionService
 * @brief Authenticates users via email and password.
 */
class SessionService
{
  public:
    SessionService() = default;
    ~SessionService() = default;

    /**
     * @brief Authenticate with email + password.
     *
     * On success the callback receives a JSON object with
     * `accessToken`, `refreshToken`, and `user`.
     *
     * @param email     Registered email address.
     * @param password  Plain-text password to verify.
     * @param onSuccess Callback with auth payload.
     * @param onError   Callback on bad credentials / error.
     */
    void loginUser(
        const std::string& email,
        const std::string& password,
        Callback onSuccess,
        ErrCallback onError);

};

} // namespace services
