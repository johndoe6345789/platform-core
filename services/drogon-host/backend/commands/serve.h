/**
 * @file serve.h
 * @brief Declaration of the serve sub-command handler.
 * @copyright 2024 Nextra Contributors
 */

#pragma once

#include <cstdint>
#include <string>

namespace commands
{

/**
 * @brief Start the Drogon HTTP server.
 *
 * Installs POSIX signal handlers, loads the Drogon JSON
 * config, registers CORS pre-sending advice, and enters
 * the Drogon event loop.
 *
 * @param port   TCP port to listen on.
 * @param config Path to the Drogon JSON config file.
 * @throws std::runtime_error if the config file is
 *         missing.
 */
void cmdServe(std::uint16_t port, const std::string& config);

} // namespace commands
