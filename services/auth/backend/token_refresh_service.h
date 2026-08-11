#pragma once
/**
 * @file token_refresh_service.h
 * @brief Access-token refresh using a valid refresh JWT.
 */

#include "auth_service_types.h"

#include <string>

namespace services
{

/**
 * @class TokenRefreshService
 * @brief Validates a refresh JWT and issues a new access token.
 */
class TokenRefreshService
{
  public:
    TokenRefreshService() = default;
    ~TokenRefreshService() = default;

    /**
     * @brief Refresh an expired access token.
     * @param refreshToken A valid, non-blocked refresh JWT.
     * @param onSuccess    Callback with `{accessToken}`.
     * @param onError      Callback on invalid / blocked token.
     */
    void refreshAccessToken(
        const std::string& refreshToken,
        Callback onSuccess,
        ErrCallback onError);
};

} // namespace services
