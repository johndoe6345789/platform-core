/**
 * @file LintCmd.cpp
 * @brief Implementation of the lint subcommand.
 */

#include "LintCmd.h"

#include <fmt/core.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace manager
{

/// Directories to scan for source files.
static const std::vector<std::string> kScanDirs = {"src", "tests", "include"};

/// File extensions to include.
static const std::vector<std::string> kExtensions = {".cpp", ".h", ".hpp",
                                                     ".cc", ".cxx"};

static bool is_cpp_file(const std::string& ext)
{
    return std::find(kExtensions.begin(), kExtensions.end(), ext) !=
           kExtensions.end();
}

static std::vector<std::string> find_sources()
{
    std::vector<std::string> files;
    for (const auto& dir : kScanDirs) {
        if (!fs::exists(dir))
            continue;
        for (const auto& e : fs::recursive_directory_iterator(dir)) {
            if (!e.is_regular_file())
                continue;
            if (is_cpp_file(e.path().extension().string()))
                files.push_back(e.path().string());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

static int run(const std::string& cmd)
{
    fmt::print("[lint] $ {}\n", cmd);
    return std::system(cmd.c_str());
}

int LintCmd::lint()
{
    auto files = find_sources();
    if (files.empty()) {
        fmt::print("[lint] No source files found.\n");
        return 0;
    }
    fmt::print("[lint] Checking {} file(s)...\n", files.size());
    int result = 0;
    for (const auto& file : files) {
        auto cmd = fmt::format("clang-format --dry-run --Werror {}", file);
        int rc = run(cmd);
        if (rc != 0)
            result = rc;
    }
    if (fs::exists("build/Debug/compile_commands.json")) {
        for (const auto& file : files) {
            auto cmd = fmt::format("clang-tidy -p build/Debug {}", file);
            int rc = run(cmd);
            if (rc != 0)
                result = rc;
        }
    } else {
        fmt::print("[lint] compile_commands.json not "
                   "found; skipping clang-tidy.\n");
    }
    if (result == 0)
        fmt::print("[lint] All checks passed.\n");
    else
        fmt::print("[lint] Issues found.\n");
    return result;
}

} // namespace manager
