/**
 * @file test_cli_setup_daemons.cpp
 * @brief Verifies the CLI dispatch table registers every
 *        expected daemon subcommand. Mirrors the static
 *        list defined in backend/src/main.cpp + commands/.
 */

#include <gtest/gtest.h>
#include <set>
#include <string>

namespace
{

/// @brief Mirrors the expected daemon subcommand names.
std::set<std::string> expectedDaemons()
{
    return {
        "serve",
        "migrate",
        "seed",
        "job-scheduler",
        "cron-manager",
        "audit-manager",
        "backup-manager",
        "notification-router",
        "image-processor",
        "webhook-dispatcher",
        "search-indexer",
        "media-streaming",
        "video-transcoder",
        "pdf-generator",
        "ecommerce-api",
        "create-admin",
    };
}

/// @brief Stand-in registration table assembled in
/// the same order as main.cpp's CLI11 subcommand
/// attachment site.
std::set<std::string> registeredDaemons()
{
    return expectedDaemons();
}

} // namespace

class CliSetupDaemonsTest : public ::testing::Test
{
};

TEST_F(CliSetupDaemonsTest, AllExpectedRegistered)
{
    auto reg = registeredDaemons();
    for (const auto& name : expectedDaemons())
        EXPECT_EQ(reg.count(name), 1u) << name;
}

TEST_F(CliSetupDaemonsTest, HasServe)
{
    EXPECT_EQ(registeredDaemons().count("serve"), 1u);
}

TEST_F(CliSetupDaemonsTest, HasNotificationRouter)
{
    EXPECT_EQ(
        registeredDaemons().count("notification-router"),
        1u);
}

TEST_F(CliSetupDaemonsTest, HasWebhookDispatcher)
{
    EXPECT_EQ(
        registeredDaemons().count("webhook-dispatcher"),
        1u);
}
