#pragma once
/**
 * @file token_service.h
 * @brief Token logout and blocklist management.
 */

#include "auth_service_types.h"

#include <functional>
#include <string>

namespace services
{

/**
 * @class TokenService
 * @brief Manages token revocation and blocklist queries.
 */
class TokenService
{
  public:
    TokenService() = default;
    ~TokenService() = default;

    /**
     * @brief Log the user out by blocking the token JTI.
     * @param jti       The JWT ID claim to block.
     * @param onSuccess Callback on success (empty JSON).
     * @param onError   Callback on failure.
     */
    void logoutUser(
        const std::string& jti,
        Callback onSuccess,
        ErrCallback onError);

    /**
     * @brief Check whether a JTI has been blocklisted.
     * @param jti       The JWT ID to check.
     * @param callback  Receives `true` if blocked.
     */
    void isTokenBlocked(
        const std::string& jti,
        std::function<void(bool)> callback);
};

} // namespace services
