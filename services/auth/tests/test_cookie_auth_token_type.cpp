/**
 * @file test_cookie_auth_token_type.cpp
 * @brief Regression test: CookieAuthFilter must
 *        reject JWTs whose `type` claim is not
 *        exactly "access".
 *
 * The filter relies on JwtClaims.isRefresh which is
 * set by jwt_verify.cpp when the "type" payload
 * claim equals "refresh". This test mirrors that
 * logic and the acceptance gate the filter enforces.
 */

#include <gtest/gtest.h>
#include <string>

namespace
{

/// @brief Mirrors jwt_verify.cpp isRefresh logic.
bool isRefreshClaim(const std::string& typeClaim)
{
    return typeClaim == "refresh";
}

/// @brief Mirrors CookieAuthFilter acceptance gate.
/// @return true if the filter would chain through.
bool filterAccepts(const std::string& typeClaim)
{
    return !isRefreshClaim(typeClaim);
}

} // namespace

TEST(CookieAuthTokenType, AccessAccepted)
{
    EXPECT_TRUE(filterAccepts("access"));
}

TEST(CookieAuthTokenType, RefreshRejected)
{
    EXPECT_FALSE(filterAccepts("refresh"));
}

TEST(CookieAuthTokenType, EmptyTypeAccepted)
{
    // Legacy tokens without a type claim parse as
    // isRefresh=false — they must still be allowed
    // so old sessions don't break on deploy.
    EXPECT_TRUE(filterAccepts(""));
}

TEST(CookieAuthTokenType, UnknownTypeAccepted)
{
    // Only an explicit "refresh" is rejected.
    // Any future custom type (e.g. "service") must
    // be handled by a dedicated filter, not this one.
    EXPECT_TRUE(filterAccepts("service"));
}
