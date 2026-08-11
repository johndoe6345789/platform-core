/**
 * @file test_admin_identity.cpp
 * @brief Username derivation for create-admin.
 */

#include "users/backend/commands/admin_identity.h"

#include <gtest/gtest.h>

namespace
{

using commands::admin::usernameFromEmail;

TEST(AdminIdentityTest, NormalisesEmailLocalPart)
{
    EXPECT_EQ(usernameFromEmail("Jane.Doe+ops@example.com"),
              "jane_doe_ops");
}

TEST(AdminIdentityTest, FallsBackForShortNames)
{
    EXPECT_EQ(usernameFromEmail("a@b.co"), "admin");
}

} // namespace
