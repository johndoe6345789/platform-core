/**
 * @file DockerStatusCmd.h
 * @brief Docker status dashboard showing engine info,
 *        images, containers, and buildx builders.
 */

#pragma once

#include <CLI/CLI.hpp>

namespace manager
{

/**
 * @class DockerStatusCmd
 * @brief Queries Docker state and prints a concise
 *        health dashboard to the terminal.
 */
class DockerStatusCmd
{
  public:
    /**
     * @brief Register the status subcommand.
     * @param docker The parent docker CLI::App.
     */
    static void registerSub(CLI::App& docker);

    /**
     * @brief Run the status check.
     * @return int Always 0 (informational only).
     */
    static int execute();
};

} // namespace manager
