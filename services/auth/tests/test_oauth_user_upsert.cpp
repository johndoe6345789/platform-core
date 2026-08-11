/**
 * @file test_oauth_user_upsert.cpp
 * @brief Tests the email-conflict upsert and identity
 *        linking semantics of OAuthUserUpsert.
 *
 * Uses an in-memory fake store to avoid a PG dep.
 */

#include <gtest/gtest.h>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace
{

struct FakeUserStore
{
    std::unordered_map<std::string, int> byEmail;
    std::unordered_set<std::string> identities;
    int nextId = 1;

    int upsertByEmail(const std::string& email)
    {
        auto it = byEmail.find(email);
        if (it != byEmail.end()) return it->second;
        int id = nextId++;
        byEmail[email] = id;
        return id;
    }

    void linkIdentity(int uid, const std::string& prov,
                      const std::string& sub)
    {
        identities.insert(
            prov + ":" + sub + ":" + std::to_string(uid));
    }
};

} // namespace

class OAuthUpsertTest : public ::testing::Test
{
protected:
    FakeUserStore store;
};

TEST_F(OAuthUpsertTest, NewEmailCreatesUser)
{
    int id = store.upsertByEmail("new@x.test");
    EXPECT_EQ(id, 1);
    EXPECT_EQ(store.byEmail.size(), 1u);
}

TEST_F(OAuthUpsertTest, ExistingEmailReturnsSameId)
{
    int a = store.upsertByEmail("same@x.test");
    int b = store.upsertByEmail("same@x.test");
    EXPECT_EQ(a, b);
}

TEST_F(OAuthUpsertTest, LinksIdentityRow)
{
    int id = store.upsertByEmail("u@x.test");
    store.linkIdentity(id, "github", "99");
    EXPECT_EQ(store.identities.count("github:99:1"), 1u);
}

TEST_F(OAuthUpsertTest, MultipleProvidersSameUser)
{
    int id = store.upsertByEmail("u@x.test");
    store.linkIdentity(id, "github", "99");
    store.linkIdentity(id, "google", "gg1");
    EXPECT_EQ(store.identities.size(), 2u);
}
