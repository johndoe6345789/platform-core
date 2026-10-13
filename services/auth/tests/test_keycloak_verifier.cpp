/**
 * @file test_keycloak_verifier.cpp
 * @brief Round-trip test: sign with RS256, verify via
 *        KeycloakVerifier using an injected public key.
 */

#include "auth/backend/keycloak/JwksClient.h"
#include "auth/backend/keycloak/KeycloakVerifier.h"
#include "test_keycloak_verifier_helpers.h"

#define JWT_DISABLE_PICOJSON
#include <jwt-cpp/traits/nlohmann-json/defaults.h>

#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <string>

namespace kc = services::auth::keycloak;

namespace
{
constexpr const char* kIss = "https://sso.test/api/dbal";
constexpr const char* kClient = "test-app";

class KeycloakVerifier : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        setenv("OIDC_ISSUER", kIss, 1);
        setenv("OIDC_CLIENT_ID", kClient, 1);
        jwks.injectForTest("kid", kp.pub);
    }

    std::string sign(jwt::builder<jwt::traits::nlohmann_json> b)
    {
        return b.set_key_id("kid").sign(
            jwt::algorithm::rs256(kp.pub, kp.priv, "", ""));
    }

    jwt::builder<jwt::traits::nlohmann_json> fresh(
        const std::string& sub)
    {
        auto now = std::chrono::system_clock::now();
        return jwt::create()
            .set_issuer(kIss)
            .set_audience(kClient)
            .set_subject(sub)
            .set_issued_at(now)
            .set_expires_at(now + std::chrono::minutes{5});
    }

    test_kc::KeyPair kp = test_kc::makeRsa();
    kc::JwksClient jwks{"unused"};
    kc::KeycloakVerifier v{jwks};
};
} // namespace

TEST_F(KeycloakVerifier, AcceptsDbalToken)
{
    auto token = sign(fresh("alice@x.io").set_payload_claim(
        "role", jwt::claim(std::string{"admin"})));

    auto out = v.verify(token);
    ASSERT_TRUE(out.has_value());
    EXPECT_EQ(out->sub, "alice@x.io");
    EXPECT_EQ(out->userId, kc::userIdFor(kIss, "alice@x.io"));
    ASSERT_EQ(out->roles.size(), 1u);
    EXPECT_EQ(out->roles[0], "admin");
}

TEST_F(KeycloakVerifier, RejectsExpired)
{
    auto past = std::chrono::system_clock::now()
                - std::chrono::hours{2};
    auto token = sign(jwt::create()
                          .set_issuer(kIss)
                          .set_audience(kClient)
                          .set_subject("u")
                          .set_issued_at(past)
                          .set_expires_at(
                              past + std::chrono::minutes{5}));
    EXPECT_FALSE(v.verify(token).has_value());
}

TEST_F(KeycloakVerifier, RejectsBadAudience)
{
    auto token = sign(fresh("u").set_audience("other-app"));
    EXPECT_FALSE(v.verify(token).has_value());
}

TEST_F(KeycloakVerifier, RejectsOtherIssuer)
{
    auto token = sign(fresh("u").set_issuer("https://evil"));
    EXPECT_FALSE(v.verify(token).has_value());
}

TEST(UserIdFor, KeepsUuidSubjects)
{
    const std::string id = "123e4567-e89b-12d3-a456-426614174000";
    EXPECT_EQ(kc::userIdFor("iss", id), id);
}

TEST(UserIdFor, DerivesStableUuidV5)
{
    const auto a = kc::userIdFor("iss", "alice");
    EXPECT_EQ(a, kc::userIdFor("iss", "alice"));
    EXPECT_NE(a, kc::userIdFor("iss", "bob"));
    EXPECT_NE(a, kc::userIdFor("other", "alice"));
    ASSERT_EQ(a.size(), 36u);
    EXPECT_EQ(a[14], '5');
}
