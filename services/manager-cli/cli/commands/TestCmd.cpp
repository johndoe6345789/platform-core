/**
 * @file TestCmd.cpp
 * @brief Implementation of the test subcommand.
 */

#include "TestCmd.h"
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
    fmt::print("[test] $ {}\n", cmd);
    return std::system(cmd.c_str());
}

int TestCmd::execute(const std::string& filter)
{
    const std::string build_dir = "build/Debug";

    fmt::print("[test] Building tests...\n");

    // Build in debug mode for testing.
    int rc = BuildCmd::execute(false);
    if (rc != 0) {
        fmt::print("[test] Build failed, cannot run tests\n");
        return rc;
    }

    // Run ctest from the build directory.
    std::string cmd = fmt::format("ctest --test-dir {} "
                                  "--output-on-failure --parallel",
                                  build_dir);

    if (!filter.empty()) {
        cmd += fmt::format(" --tests-regex \"{}\"", filter);
    }

    fmt::print("[test] Running tests...\n");
    rc = run(cmd);

    if (rc == 0) {
        fmt::print("[test] All tests passed.\n");
    } else {
        fmt::print("[test] Some tests failed (exit {})\n", rc);
    }

    return rc;
}

} // namespace manager
