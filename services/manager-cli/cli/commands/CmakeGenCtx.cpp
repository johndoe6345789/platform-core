/**
 * @file CmakeGenCtx.cpp
 * @brief Build inja context from project.json for cmake
 *        generation.
 */

#include "CmakeGenCmd.h"
#include "CmakeGenDiscover.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace manager
{

json CmakeGenCmd::buildContext(const json& project, const fs::path& base)
{
    json ctx = project;
    bool has_tests = false;

    for (auto& tgt : ctx["targets"]) {
        std::vector<std::string> inc, exc;
        for (const auto& p : tgt["glob_patterns"])
            inc.push_back(p.get<std::string>());
        for (const auto& p : tgt["exclude_patterns"])
            exc.push_back(p.get<std::string>());

        auto srcs = discover_sources(
            base, tgt["sources_dir"].get<std::string>(), inc, exc);

        tgt["source_files"] = json::array();
        for (const auto& s : srcs)
            tgt["source_files"].push_back(s);

        if (tgt["name"].get<std::string>().find("test") != std::string::npos)
            has_tests = true;
    }
    ctx["has_tests"] = has_tests;
    return ctx;
}

} // namespace manager
