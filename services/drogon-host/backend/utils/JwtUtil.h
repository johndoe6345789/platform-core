#pragma once
/**
 * @file JwtUtil.h
 * @brief JWT token generation and verification utilities.
 */

#include <string>

namespace utils
{

/**
 * @brief Generate a short-lived access token (15 min).
 * @param userId  Unique user identifier.
 * @param role    User role (e.g. "user", "admin").
 * @return Signed JWT string.
 */
[[nodiscard]] auto generateAccessToken(const std::string& userId,
                                       const std::string& role) -> std::string;

/**
 * @brief Generate a long-lived refresh token (30 days).
 * @param userId  Unique user identifier.
 * @return Signed JWT string.
 */
[[nodiscard]] auto
generateRefreshToken(const std::string& userId) -> std::string;

/**
 * @brief Decoded JWT claims.
 */
struct JwtClaims {
    std::string userId;
    std::string role;
    bool isRefresh{false};
};

/**
 * @brief Verify and decode a JWT token.
 * @param token  Raw JWT string.
 * @return Decoded claims.
 * @throws std::runtime_error on invalid / expired token.
 */
[[nodiscard]] auto verifyToken(const std::string& token) -> JwtClaims;

/**
 * @brief Retrieve the JWT signing secret.
 * @return Secret from custom_config, JWT_SECRET env, or dev default.
 */
[[nodiscard]] auto getSecret() -> std::string;

/**
 * @brief Retrieve the JWT issuer string.
 * @return Issuer from custom_config or "next-extra" default.
 */
[[nodiscard]] auto getIssuer() -> std::string;

} // namespace utils
