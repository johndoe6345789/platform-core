/**
 * @file jwt_verify.cpp
 * @brief JWT token verification logic.
 */
#include "JwtUtil.h"
#include "jwt_config.h"

#define JWT_DISABLE_PICOJSON
#include <jwt-cpp/traits/nlohmann-json/defaults.h>

namespace utils
{

auto verifyToken(
    const std::string& token) -> JwtClaims
{
    auto verifier =
        jwt::verify()
            .allow_algorithm(
                jwt::algorithm::hs256{getSecret()})
            .with_issuer(getIssuer());
    auto decoded = jwt::decode(token);
    verifier.verify(decoded);
    JwtClaims claims;
    claims.userId = decoded.get_subject();
    if (decoded.has_payload_claim("role")) {
        claims.role =
            decoded.get_payload_claim("role")
                .as_string();
    }
    if (decoded.has_payload_claim("type")) {
        claims.isRefresh =
            decoded.get_payload_claim("type")
                .as_string() == "refresh";
    }
    return claims;
}

} // namespace utils
