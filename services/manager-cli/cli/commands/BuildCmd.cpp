/**
 * @file BuildCmd.cpp
 * @brief Implementation of the build subcommand.
 */

#include "BuildCmd.h"

#include <fmt/core.h>

#include <cstdlib>
#include <string>

namespace manager
{

/**
 * @brief Run a shell command, printing it first.
 *
 * @param cmd The command string to execute.
 * @return int The process exit code.
 */
static int run(const std::string& cmd)
{
    fmt::print("[build] $ {}\n", cmd);
    return std::system(cmd.c_str());
}

int BuildCmd::execute(bool release)
{
    const std::string build_type = release ? "Release" : "Debug";
    const std::string build_dir = "build/" + build_type;

    fmt::print("[build] Starting {} build...\n", build_type);

    // Step 1: Conan install.
    {
        std::string cmd = fmt::format("conan install . "
                                      "--output-folder={} "
                                      "--build=missing "
                                      "-s build_type={}",
                                      build_dir, build_type);
        int rc = run(cmd);
        if (rc != 0) {
            fmt::print("[build] conan install failed\n");
            return rc;
        }
    }

    // Step 2: CMake configure.
    {
        std::string cmd = fmt::format("cmake -B {} "
                                      "-DCMAKE_BUILD_TYPE={} "
                                      "-DCMAKE_TOOLCHAIN_FILE="
                                      "{}/conan_toolchain.cmake",
                                      build_dir, build_type, build_dir);
        int rc = run(cmd);
        if (rc != 0) {
            fmt::print("[build] cmake configure failed\n");
            return rc;
        }
    }

    // Step 3: CMake build.
    {
        std::string cmd = fmt::format("cmake --build {} --parallel", build_dir);
        int rc = run(cmd);
        if (rc != 0) {
            fmt::print("[build] cmake build failed\n");
            return rc;
        }
    }

    fmt::print("[build] {} build complete.\n", build_type);
    return 0;
}

} // namespace manager
