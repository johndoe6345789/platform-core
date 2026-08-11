/**
 * @file DockerCmd.h
 * @brief Docker subcommand hub - registers compose ops,
 *        smart deps builder, and status dashboard as
 *        nested CLI11 subcommands under `docker`.
 */

#pragma once

#include <CLI/CLI.hpp>

namespace manager
{

/**
 * @class DockerCmd
 * @brief Registers all docker-related subcommands on the
 *        parent CLI::App and dispatches compose actions.
 */
class DockerCmd
{
  public:
    /**
     * @brief Register every docker subcommand.
     *
     * Creates `docker build|up|down|logs|deps|status`
     * under the given parent app.
     *
     * @param parent The root CLI::App.
     */
    static void registerAll(CLI::App& parent);

    /**
     * @brief Run a docker compose action by name.
     * @param action One of: build, up, down, logs.
     * @return int 0 on success, non-zero on failure.
     */
    static int compose(const std::string& action);
};

} // namespace manager
