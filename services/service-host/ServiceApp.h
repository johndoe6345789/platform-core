#pragma once
/**
 * @file ServiceApp.h
 * @brief Shared Drogon microservice bootstrap.
 *        Every nextra-* service binary calls
 *        runService() instead of invoking the
 *        full CLI dispatch used by the monolith.
 */
#include <string>

namespace services
{

/**
 * @brief Configure Drogon from config file and run.
 *
 * Installs SIGINT/SIGTERM handlers, loads the JSON
 * config, then calls drogon::app().run() which
 * blocks until quit() is called.
 *
 * @param configPath Path to Drogon JSON config.
 * @param name       Service name used in log lines.
 */
void runService(const std::string& configPath,
                const std::string& name);

} // namespace services
