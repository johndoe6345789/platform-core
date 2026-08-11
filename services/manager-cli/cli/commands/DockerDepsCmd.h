/**
 * @file DockerDepsCmd.h
 * @brief Build base dependency images with multi-arch
 *        awareness and automatic registry detection.
 */

#pragma once

#include <CLI/CLI.hpp>

#include <string>

namespace manager
{

/**
 * @class DockerDepsCmd
 * @brief Builds frontend-deps and/or backend-deps images
 *        using docker buildx with platform detection.
 */
class DockerDepsCmd
{
  public:
    /**
     * @brief Register the deps subcommand under docker.
     * @param docker The parent docker CLI::App.
     */
    static void registerSub(CLI::App& docker);

    /**
     * @brief Execute the deps image build.
     * @param target  "backend", "frontend", or "all".
     * @param platform Target platform(s), e.g.
     *                 "linux/amd64,linux/arm64".
     * @param push    If true, push to the registry.
     * @return int 0 on success, non-zero on failure.
     */
    static int execute(const std::string& target, const std::string& platform,
                       bool push);
};

} // namespace manager
