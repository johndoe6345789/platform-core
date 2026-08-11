/**
 * @file CmakeGenDiscover.h
 * @brief Source file discovery for cmake-gen.
 */

#pragma once

#include "CmakeGenGlob.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace manager
{

/// @brief Walk a directory tree and collect source files
///        matching include globs, filtering out excludes.
inline std::vector<std::string>
discover_sources(const fs::path& base_dir, const std::string& src_dir,
                 const std::vector<std::string>& includes,
                 const std::vector<std::string>& excludes)
{
    std::vector<std::string> result;
    fs::path search = base_dir / src_dir;

    if (!fs::exists(search)) {
        std::cerr << "[cmake-gen] warn: " << search << " missing\n";
        return result;
    }

    for (const auto& e : fs::recursive_directory_iterator(search)) {
        if (!e.is_regular_file())
            continue;
        auto name = e.path().filename().string();
        if (!matches_any(name, includes))
            continue;
        if (!excludes.empty() && matches_any(name, excludes))
            continue;
        result.push_back(fs::relative(e.path(), base_dir).generic_string());
    }

    std::sort(result.begin(), result.end());
    return result;
}

} // namespace manager
