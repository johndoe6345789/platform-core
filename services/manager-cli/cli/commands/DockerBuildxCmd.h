/**
 * @file DockerBuildxCmd.h
 * @brief Local buildx builds for app images, auto-wiring
 *        the pre-baked deps images as DEPS_IMAGE ARG.
 */

#pragma once

#include <CLI/CLI.hpp>

#include <string>

namespace manager
{

/**
 * @class DockerBuildxCmd
 * @brief Builds frontend and/or backend app images with
 *        buildx, injecting the matching deps image when
 *        available locally.
 */
class DockerBuildxCmd
{
  public:
    /**
     * @brief Register the buildx subcommand.
     * @param docker The parent docker CLI::App.
     */
    static void registerSub(CLI::App& docker);

    /**
     * @brief Execute a local buildx app build.
     * @param target   "backend", "frontend", or "all".
     * @param platform Target platform string.
     * @param noDeps   Skip deps image injection.
     * @return int 0 on success, non-zero on failure.
     */
    static int execute(const std::string& target, const std::string& platform,
                       bool noDeps);
};

} // namespace manager
