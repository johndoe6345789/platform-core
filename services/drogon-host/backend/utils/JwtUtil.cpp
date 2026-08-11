/**
 * @file JwtUtil.cpp
 * @brief JWT token generation.
 *
 * Verification logic is in jwt_verify.cpp.
 */
#include "JwtUtil.h"
#include "jwt_config.h"

#include <chrono>
#define JWT_DISABLE_PICOJSON
#include <jwt-cpp/traits/nlohmann-json/defaults.h>

namespace utils
{

auto getSecret() -> std::string
{
    return getJwtSecret();
}

auto getIssuer() -> std::string
{
    return getJwtIssuer();
}

auto generateAccessToken(
    const std::string& userId,
    const std::string& role) -> std::string
{
    auto now = std::chrono::system_clock::now();
    return jwt::create()
        .set_issuer(getIssuer())
        .set_type("JWT")
        .set_subject(userId)
        .set_payload_claim(
            "role", jwt::claim(role))
        .set_payload_claim(
            "type",
            jwt::claim(std::string{"access"}))
        .set_issued_at(now)
        .set_expires_at(
            now + std::chrono::minutes{
                      kAccessMinutes})
        .sign(jwt::algorithm::hs256{
            getSecret()});
}

auto generateRefreshToken(
    const std::string& userId) -> std::string
{
    auto now = std::chrono::system_clock::now();
    return jwt::create()
        .set_issuer(getIssuer())
        .set_type("JWT")
        .set_subject(userId)
        .set_payload_claim(
            "type",
            jwt::claim(std::string{"refresh"}))
        .set_issued_at(now)
        .set_expires_at(
            now + std::chrono::hours{
                      kRefreshDays * 24})
        .sign(jwt::algorithm::hs256{
            getSecret()});
}

} // namespace utils
