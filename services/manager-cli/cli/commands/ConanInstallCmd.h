/**
 * @file ConanInstallCmd.h
 * @brief Conan install with automatic platform_tool_requires
 *        fix for exotic architectures (riscv64, ppc64le).
 */

#pragma once

#include <string>
#include <vector>

namespace manager
{

/**
 * @class ConanInstallCmd
 * @brief Runs conan install, retrying with cmake pinned in
 *        platform_tool_requires when binaries are unavailable.
 */
class ConanInstallCmd
{
  public:
    /**
     * @brief Execute conan install with two-pass fallback.
     *
     * First pass runs conan install normally. If it fails
     * because cmake binaries are unavailable for the current
     * arch, the resolved cmake version is extracted from the
     * error and pinned in the default profile's
     * [platform_tool_requires], then conan install is retried.
     *
     * @param args Extra arguments forwarded to conan install.
     * @return int 0 on success, non-zero on failure.
     */
    static int execute(const std::vector<std::string>& args);
};

} // namespace manager
