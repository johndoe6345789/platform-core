#pragma once
/**
 * @file auth_service_facade.h
 * @brief AuthService method declarations for the
 *        facade class.
 *
 * These are the public method signatures that
 * delegate to focused sub-services. The class
 * definition and member variables live in
 * AuthService.h.
 */

#include "auth_service_types.h"
#include <functional>
#include <string>

namespace services
{

/**
 * @brief Forward declarations only -- the actual
 *        method bodies live in AuthService.cpp.
 *
 * This header exists so AuthService.h stays under
 * the 100-line file-size limit.  It is included
 * by AuthService.h and should not be included
 * directly elsewhere.
 */

/** @brief Register a new user. */
void registerUser(
    const std::string& email,
    const std::string& username,
    const std::string& password,
    const std::string& displayName,
    Callback onSuccess, ErrCallback onError);

/** @brief Authenticate with email + password. */
void loginUser(
    const std::string& email,
    const std::string& password,
    Callback onSuccess, ErrCallback onError);

/** @brief Refresh an expired access token. */
void refreshAccessToken(
    const std::string& refreshToken,
    Callback onSuccess, ErrCallback onError);

/** @brief Block a token JTI (logout). */
void logoutUser(
    const std::string& jti,
    Callback onSuccess, ErrCallback onError);

/** @brief Confirm email via one-time token. */
void confirmEmail(
    const std::string& token,
    Callback onSuccess, ErrCallback onError);

/** @brief Request a password-reset email. */
void requestPasswordReset(
    const std::string& email,
    Callback onSuccess, ErrCallback onError);

/** @brief Execute a password reset. */
void resetPassword(
    const std::string& token,
    const std::string& newPassword,
    Callback onSuccess, ErrCallback onError);

/** @brief Check if a JTI is blocklisted. */
void isTokenBlocked(
    const std::string& jti,
    std::function<void(bool)> callback);

} // namespace services
