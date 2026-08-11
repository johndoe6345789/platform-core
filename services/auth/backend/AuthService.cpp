/**
 * @file AuthService.cpp
 * @brief Thin coordinator delegating to sub-services.
 */
#include "auth/backend/AuthService.h"

namespace services
{

void AuthService::registerUser(
    const std::string& email,
    const std::string& username,
    const std::string& password,
    const std::string& displayName,
    Callback onSuccess, ErrCallback onError)
{
    registration_.registerUser(
        email, username, password, displayName,
        onSuccess, onError);
}

void AuthService::loginUser(
    const std::string& email,
    const std::string& password,
    Callback onSuccess, ErrCallback onError)
{
    session_.loginUser(
        email, password, onSuccess, onError);
}

void AuthService::refreshAccessToken(
    const std::string& refreshToken,
    Callback onSuccess, ErrCallback onError)
{
    tokenRefresh_.refreshAccessToken(
        refreshToken, onSuccess, onError);
}

void AuthService::logoutUser(
    const std::string& jti,
    Callback onSuccess, ErrCallback onError)
{
    token_.logoutUser(jti, onSuccess, onError);
}

void AuthService::confirmEmail(
    const std::string& token,
    Callback onSuccess, ErrCallback onError)
{
    emailConfirm_.confirmEmail(
        token, onSuccess, onError);
}

void AuthService::requestPasswordReset(
    const std::string& email,
    Callback onSuccess, ErrCallback onError)
{
    pwResetReq_.execute(email, onSuccess, onError);
}

void AuthService::resetPassword(
    const std::string& token,
    const std::string& newPassword,
    Callback onSuccess, ErrCallback onError)
{
    pwResetExec_.execute(
        token, newPassword, onSuccess, onError);
}

void AuthService::isTokenBlocked(
    const std::string& jti,
    std::function<void(bool)> callback)
{
    token_.isTokenBlocked(jti, callback);
}

} // namespace services
