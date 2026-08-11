/**
 * @file serve.cpp
 * @brief Implementation of the serve sub-command.
 * @copyright 2024 Nextra Contributors
 */

#include "serve.h"
#include "serve_advice.h"

#include <drogon/drogon.h>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

#include <csignal>
#include <filesystem>
#include <stdexcept>
#include <string>

namespace fs = std::filesystem;

namespace
{

/// @brief Gracefully shut down Drogon on SIGINT / SIGTERM.
void signalHandler(int signum)
{
    spdlog::info(
        "Received signal {} -- shutting down gracefully.",
        signum);
    drogon::app().quit();
}

/// @brief Register POSIX signal handlers.
void installSignalHandlers()
{
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

} // anonymous namespace

namespace commands
{

void cmdServe(std::uint16_t port,
              const std::string& config)
{
    installSignalHandlers();

    if (!fs::exists(config)) {
        throw std::runtime_error(
            fmt::format("Config file not found: {}",
                        config));
    }

    spdlog::info("Loading configuration from: {}",
                 config);
    drogon::app().loadConfigFile(config);

    // CLI --port overrides the config value.
    drogon::app().addListener("0.0.0.0", port);

    registerHttpAdvice();

    spdlog::info("Starting nextra-api on port {}",
                 port);
    drogon::app().run();

    spdlog::info("Server stopped.");
}

} // namespace commands
