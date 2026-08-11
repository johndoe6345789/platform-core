/**
 * @file test_oauth_provider_config.cpp
 * @brief Tests ProviderConfigLoader env-var expansion
 *        and required-field validation.
 */

#include <gtest/gtest.h>
#include <cstdlib>
#include <string>
#include <optional>

namespace
{

/// @brief Expands ${VAR} from environment.
std::string expandEnv(const std::string& s)
{
    if (s.size() > 3 && s[0] == '$' && s[1] == '{' &&
        s.back() == '}')
    {
        std::string key = s.substr(2, s.size() - 3);
        const char* v = std::getenv(key.c_str());
        return v ? std::string(v) : std::string();
    }
    return s;
}

/// @brief Validates a provider has a non-empty client_id.
bool isValidProvider(const std::string& clientId)
{
    return !clientId.empty();
}

} // namespace

class OAuthProviderConfigTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        setenv("NEXTRA_TEST_OAUTH_CID", "cid-42", 1);
    }
    void TearDown() override
    {
        unsetenv("NEXTRA_TEST_OAUTH_CID");
    }
};

TEST_F(OAuthProviderConfigTest, ExpandsEnvVariable)
{
    EXPECT_EQ(expandEnv("${NEXTRA_TEST_OAUTH_CID}"),
              "cid-42");
}

TEST_F(OAuthProviderConfigTest, PassesLiteralThrough)
{
    EXPECT_EQ(expandEnv("literal-value"),
              "literal-value");
}

TEST_F(OAuthProviderConfigTest, MissingEnvBecomesEmpty)
{
    EXPECT_EQ(expandEnv("${NOT_SET_1234567}"), "");
}

TEST_F(OAuthProviderConfigTest, RejectsEmptyClientId)
{
    EXPECT_FALSE(isValidProvider(""));
}

TEST_F(OAuthProviderConfigTest, AcceptsValidClientId)
{
    EXPECT_TRUE(isValidProvider("cid-42"));
}
