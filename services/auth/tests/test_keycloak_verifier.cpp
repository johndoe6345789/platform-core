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
#include <string>

namespace kc = services::auth::keycloak;

TEST(KeycloakVerifier, AcceptsValidToken)
{
    auto kp = test_kc::makeRsa();
    kc::JwksClient jwks{"unused"};
    jwks.injectForTest("test-kid", kp.pub);
    kc::KeycloakVerifier v{jwks};

    auto now = std::chrono::system_clock::now();
    nlohmann::json roles = {{"roles", {"user", "admin"}}};
    auto token =
        jwt::create()
            .set_key_id("test-kid")
            .set_type("JWT")
            .set_issuer(kc::kKcIssuer)
            .set_audience("nextra-app")
            .set_subject("user-123")
            .set_issued_at(now)
            .set_expires_at(now + std::chrono::minutes{5})
            .set_payload_claim(
                "preferred_username",
                jwt::claim(std::string{"alice"}))
            .set_payload_claim("email",
                jwt::claim(std::string{"a@x.io"}))
            .set_payload_claim("realm_access",
                jwt::claim(roles))
            .sign(jwt::algorithm::rs256(
                kp.pub, kp.priv, "", ""));

    auto out = v.verify(token);
    ASSERT_TRUE(out.has_value());
    EXPECT_EQ(out->sub, "user-123");
    EXPECT_EQ(out->email, "a@x.io");
    EXPECT_EQ(out->preferredUsername, "alice");
    ASSERT_EQ(out->roles.size(), 2u);
}

TEST(KeycloakVerifier, RejectsExpired)
{
    auto kp = test_kc::makeRsa();
    kc::JwksClient jwks{"unused"};
    jwks.injectForTest("kid", kp.pub);
    kc::KeycloakVerifier v{jwks};

    auto past = std::chrono::system_clock::now()
                - std::chrono::hours{2};
    auto token = jwt::create()
        .set_key_id("kid")
        .set_issuer(kc::kKcIssuer)
        .set_audience("nextra-app")
        .set_subject("u")
        .set_issued_at(past)
        .set_expires_at(past + std::chrono::minutes{5})
        .sign(jwt::algorithm::rs256(
            kp.pub, kp.priv, "", ""));
    EXPECT_FALSE(v.verify(token).has_value());
}

TEST(KeycloakVerifier, RejectsBadAudience)
{
    auto kp = test_kc::makeRsa();
    kc::JwksClient jwks{"unused"};
    jwks.injectForTest("kid", kp.pub);
    kc::KeycloakVerifier v{jwks};

    auto now = std::chrono::system_clock::now();
    auto token = jwt::create()
        .set_key_id("kid")
        .set_issuer(kc::kKcIssuer)
        .set_audience("other-app")
        .set_subject("u")
        .set_issued_at(now)
        .set_expires_at(now + std::chrono::minutes{5})
        .sign(jwt::algorithm::rs256(
            kp.pub, kp.priv, "", ""));
    EXPECT_FALSE(v.verify(token).has_value());
}
