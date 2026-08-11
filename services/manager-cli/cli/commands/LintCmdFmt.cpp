/**
 * @file LintCmdFmt.cpp
 * @brief Implementation of the format subcommand.
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

/// File extensions to include.
static const std::vector<std::string> kFmtExts = {".cpp", ".h", ".hpp", ".cc",
                                                  ".cxx"};

/// Directories to scan for source files.
static const std::vector<std::string> kFmtDirs = {"src", "tests", "include"};

static std::vector<std::string> find_fmt_sources()
{
    std::vector<std::string> files;
    for (const auto& dir : kFmtDirs) {
        if (!fs::exists(dir))
            continue;
        for (const auto& e : fs::recursive_directory_iterator(dir)) {
            if (!e.is_regular_file())
                continue;
            auto ext = e.path().extension().string();
            bool match = std::find(kFmtExts.begin(), kFmtExts.end(), ext) !=
                         kFmtExts.end();
            if (match)
                files.push_back(e.path().string());
        }
    }
    std::sort(files.begin(), files.end());
    return files;
}

int LintCmd::format()
{
    auto files = find_fmt_sources();
    if (files.empty()) {
        fmt::print("[fmt] No source files found.\n");
        return 0;
    }

    fmt::print("[fmt] Formatting {} file(s)...\n", files.size());

    int result = 0;
    for (const auto& file : files) {
        auto cmd = fmt::format("clang-format -i {}", file);
        fmt::print("[fmt] $ {}\n", cmd);
        int rc = std::system(cmd.c_str());
        if (rc != 0) {
            result = rc;
            fmt::print("[fmt] Failed on: {}\n", file);
        }
    }

    if (result == 0)
        fmt::print("[fmt] Formatting complete.\n");
    return result;
}

} // namespace manager
