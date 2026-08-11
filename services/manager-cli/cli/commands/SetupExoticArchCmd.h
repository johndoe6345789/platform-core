/**
 * @file SetupExoticArchCmd.h
 * @brief Patches node_modules for riscv64 / ppc64le builds
 *        where native npm packages lack prebuilt binaries.
 */

#pragma once

#include <string>

namespace manager
{

/**
 * @class SetupExoticArchCmd
 * @brief Installs @next/swc from a cache dir, stubs
 *        @parcel/watcher, and patches @swc/core to use
 *        the WASM fallback.
 */
class SetupExoticArchCmd
{
  public:
    /**
     * @brief Run all exotic-arch fixups.
     *
     * @param swcDir   Directory with cached .node files.
     * @param nmDir    Path to node_modules.
     * @return int 0 on success, non-zero on failure.
     */
    static int execute(const std::string& swcDir, const std::string& nmDir);
};

} // namespace manager
