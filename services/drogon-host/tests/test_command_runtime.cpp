/**
 * @file test_command_runtime.cpp
 * @brief Small coverage for one-shot command path helpers.
 */

#include "drogon-host/backend/commands/command_runtime.h"

#include <gtest/gtest.h>

namespace
{

using commands::runtime::SeedKind;
using commands::runtime::seedKind;

TEST(CommandRuntimeTest, MapsSupportedSeedFiles)
{
    EXPECT_EQ(seedKind("users.json"), SeedKind::users);
    EXPECT_EQ(seedKind("badges.json"), SeedKind::badges);
    EXPECT_EQ(seedKind("feature-toggles.json"),
              SeedKind::featureToggles);
    EXPECT_EQ(seedKind("notifications.json"),
              SeedKind::notifications);
}

TEST(CommandRuntimeTest, RejectsUnsupportedSeedFiles)
{
    EXPECT_EQ(seedKind("users.legacy.json"),
              SeedKind::unsupported);
    EXPECT_EQ(seedKind("en.json"), SeedKind::unsupported);
}

} // namespace
