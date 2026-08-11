/**
 * @file SessionService.h
 * @brief JWT session token management.
 */

#pragma once

#include <optional>
#include <string>

namespace pgadmin
{

/// @brief Payload carried inside a session JWT.
struct SessionPayload {
    int userId;
    std::string username;
};

/// @brief Creates and verifies JWT session tokens.
class SessionService
{
  public:
    /// @brief Create a signed JWT for the user.
    /// @param payload User identity data
    /// @param secret HMAC-SHA256 signing key
    /// @return Signed JWT string
    static std::string createToken(
        const SessionPayload& payload,
        const std::string& secret);

    /// @brief Verify and decode a JWT.
    /// @param token The JWT string
    /// @param secret HMAC-SHA256 signing key
    /// @return Decoded payload or nullopt
    static std::optional<SessionPayload> verify(
        const std::string& token,
        const std::string& secret);
};

} // namespace pgadmin
