/**
 * @file ServiceApp.cpp
 * @brief Drogon microservice bootstrap implementation.
 */
#include "ServiceApp.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <cstdlib>

namespace services
{

namespace
{
void onSignal(int sig)
{
    spdlog::info("signal {} — shutting down", sig);
    drogon::app().quit();
}
} // anonymous namespace

void runService(const std::string& configPath,
                const std::string& name)
{
    std::signal(SIGINT,  onSignal);
    std::signal(SIGTERM, onSignal);

    spdlog::info("{}: loading {}", name, configPath);
    drogon::app().loadConfigFile(configPath);

    spdlog::info("{}: starting", name);
    drogon::app().run();
    spdlog::info("{}: stopped", name);
}

} // namespace services
