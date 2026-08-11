/**
 * @file WorkflowReg.cpp
 * @brief Discover and register JSON command files.
 */

#include "ShellUtil.h"
#include "WorkflowEngine.h"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace manager
{

static constexpr const char* kCommandsDir = ".local/commands";

/// @brief Load one command file and register it.
static void loadFile(CLI::App& app, const std::filesystem::path& path)
{
    std::ifstream in(path);
    if (!in.is_open())
        return;
    try {
        registerCmd(&app, json::parse(in));
    } catch (const std::exception& e) {
        fmt::print("[workflow] Bad {}: {}\n", path.filename().string(),
                   e.what());
    }
}

void registerWorkflows(CLI::App& app)
{
    std::filesystem::path dir;
    const char* envDir = std::getenv("MANAGER_COMMANDS_DIR");
    if (envDir && std::filesystem::exists(envDir)) {
        dir = envDir;
    } else {
        auto root = repoRoot();
        if (root.empty())
            return;
        dir = root / kCommandsDir;
        if (!std::filesystem::exists(dir))
            return;
    }
    std::vector<std::filesystem::path> files;
    for (const auto& entry : std::filesystem::directory_iterator(dir)) {
        if (entry.path().extension() == ".json")
            files.push_back(entry.path());
    }
    std::sort(files.begin(), files.end());
    for (const auto& f : files)
        loadFile(app, f);
}

} // namespace manager
