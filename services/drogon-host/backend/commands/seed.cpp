/**
 * @file seed.cpp
 * @brief Implementation of the seed sub-command.
 * @copyright 2024 Nextra Contributors
 */

#include "seed.h"
#include "command_runtime.h"
#include "seed_handlers.h"

#include <drogon/drogon.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <string>

namespace commands
{

void cmdSeed(const std::string& file)
{
    auto files = runtime::resolveSeedFiles(file);
    auto config = runtime::resolveConfigPath();
    drogon::app().loadConfigFile(config.string());
    auto db = drogon::app().getDbClient();

    for (const auto& path : files) {
        spdlog::info("Loading seed file: {}", path.string());
        switch (runtime::seedKind(path)) {
        case runtime::SeedKind::users:
            seed::loadUsers(path, db);
            break;
        case runtime::SeedKind::badges:
            seed::loadBadges(path, db);
            break;
        case runtime::SeedKind::featureToggles:
            seed::loadFeatureToggles(path, db);
            break;
        case runtime::SeedKind::notifications:
            seed::loadNotifications(path, db);
            break;
        case runtime::SeedKind::unsupported:
            break;
        }
    }
    fmt::print("Seeded {} file(s)\n", files.size());
}

} // namespace commands
