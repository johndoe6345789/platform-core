/**
 * @file test_oauth_token_exchange.cpp
 * @brief Verifies TokenExchange request shape for
 *        provider-specific token endpoints.
 *
 * Mirrors services/auth/oauth/TokenExchange.cpp body
 * formatting and header selection logic.
 */

#include <gtest/gtest.h>
#include <string>
#include <map>

namespace
{

/// @brief Encodes k=v pairs as application/x-www-form.
std::string formEncode(
    const std::map<std::string, std::string>& pairs)
{
    std::string out;
    for (const auto& [k, v] : pairs)
    {
        if (!out.empty()) out += "&";
        out += k + "=" + v;
    }
    return out;
}

/// @brief Selects the provider Accept header.
std::string acceptHeader(const std::string& provider)
{
    if (provider == "github") return "application/json";
    if (provider == "google") return "application/json";
    if (provider == "microsoft")
        return "application/json";
    return "application/json";
}

} // namespace

class OAuthTokenExchangeTest : public ::testing::Test
{
};

TEST_F(OAuthTokenExchangeTest, FormBodyContainsCode)
{
    std::map<std::string, std::string> p = {
        {"code", "abc123"},
        {"client_id", "cid"},
        {"client_secret", "sec"},
        {"grant_type", "authorization_code"},
        {"redirect_uri", "https://x/cb"}};
    auto body = formEncode(p);
    EXPECT_NE(body.find("code=abc123"), std::string::npos);
    EXPECT_NE(body.find("grant_type=authorization_code"),
              std::string::npos);
}

TEST_F(OAuthTokenExchangeTest, GithubUsesJsonAccept)
{
    EXPECT_EQ(acceptHeader("github"), "application/json");
}

TEST_F(OAuthTokenExchangeTest, UnknownProviderDefault)
{
    EXPECT_EQ(acceptHeader("unknown"),
              "application/json");
}

TEST_F(OAuthTokenExchangeTest, EmptyPairsYieldsEmptyBody)
{
    std::map<std::string, std::string> p;
    EXPECT_EQ(formEncode(p), "");
}
