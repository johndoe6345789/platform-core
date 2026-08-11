/**
 * @file command_runtime.cpp
 * @brief Shared path resolution for one-shot CLI commands.
 */

#include "command_runtime.h"

#include <algorithm>
#include <stdexcept>

namespace fs = std::filesystem;

namespace commands::runtime
{

auto repoRoot() -> fs::path
{
    auto dir = fs::current_path();
    while (true) {
        if (fs::exists(dir / ".git")) return dir;
        auto parent = dir.parent_path();
        if (parent == dir) return {};
        dir = parent;
    }
}

auto resolveConfigPath() -> fs::path
{
    auto root = repoRoot();
    for (const auto& path : {fs::path{"config/config.json"},
             root / "services/drogon-host/config/config.json"}) {
        if (!path.empty() && fs::exists(path)) return path;
    }
    throw std::runtime_error("Config file not found");
}

auto seedKind(const fs::path& path) -> SeedKind
{
    auto name = path.filename().string();
    if (name == "users.json") return SeedKind::users;
    if (name == "badges.json") return SeedKind::badges;
    if (name == "feature-toggles.json")
        return SeedKind::featureToggles;
    if (name == "notifications.json")
        return SeedKind::notifications;
    return SeedKind::unsupported;
}

auto resolveSeedFiles(const std::string& file)
    -> std::vector<fs::path>
{
    auto root = repoRoot();
    if (!file.empty()) {
        auto path = fs::exists(file) ? fs::path{file}
                                     : root / file;
        if (!fs::exists(path))
            throw std::runtime_error("Seed file not found");
        if (seedKind(path) == SeedKind::unsupported)
            throw std::runtime_error("Unsupported seed file");
        return {path};
    }

    std::vector<fs::path> out;
    for (const auto& dir : fs::directory_iterator(root / "services")) {
        auto seeds = dir.path() / "seeds";
        if (!dir.is_directory() || !fs::exists(seeds)) continue;
        for (const auto& entry : fs::directory_iterator(seeds)) {
            if (!entry.is_regular_file()) continue;
            if (seedKind(entry.path()) == SeedKind::unsupported) continue;
            out.push_back(entry.path());
        }
    }
    std::sort(out.begin(), out.end(), [](const auto& a, const auto& b) {
        return seedKind(a) < seedKind(b);
    });
    if (out.empty())
        throw std::runtime_error("No supported seed files found");
    return out;
}

} // namespace commands::runtime
