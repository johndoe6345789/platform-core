/**
 * @file ActCmdRun.cpp
 * @brief Workflow execution and listing for act commands.
 */

#include "ActCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace manager
{

/// @brief Build the full act CLI string with auto flags.
static std::string buildActCmd(const std::string& wfDir,
                               const std::string& file, const std::string& job,
                               bool verbose)
{
    auto wfPath = fmt::format("{}/{}", wfDir, file);
    auto cmd = fmt::format("act -W {}", wfPath);

    auto runners = ActCmd::loadRunners();
    for (const auto& label : ActCmd::extractRunners(wfPath)) {
        auto img = ActCmd::runnerImage(label, runners);
        cmd += fmt::format(" -P {}={}", label, img);
        fmt::print("[act] Runner: {} -> {}\n", label, img);
    }

    auto host = hostPlatform();
    if (host != "linux/amd64") {
        cmd += fmt::format(" --env BUILD_PLATFORM={}", host);
        fmt::print("[act] Injecting BUILD_PLATFORM={}\n", host);
    }

    if (!job.empty())
        cmd += fmt::format(" -j {}", job);
    if (verbose)
        cmd += " -v";
    return cmd;
}

int ActCmd::runWorkflow(const std::string& file, const std::string& job,
                        bool verbose)
{
    if (!checkAct())
        return 1;
    auto wfDir = workflowDir();
    if (wfDir.empty()) {
        fmt::print("[act] Not inside a git repository\n");
        return 1;
    }
    auto path = fmt::format("{}/{}", wfDir, file);
    if (!fs::exists(path)) {
        fmt::print("[act] Workflow not found: {}\n", path);
        return 1;
    }
    return shell("act", buildActCmd(wfDir, file, job, verbose));
}

int ActCmd::list()
{
    auto wfDir = workflowDir();
    if (wfDir.empty()) {
        fmt::print("[act] Not inside a git repository\n");
        return 1;
    }
    if (!fs::exists(wfDir)) {
        fmt::print("[act] No workflow dir: {}\n", wfDir);
        return 1;
    }
    fmt::print("[act] Workflows in {}:\n", wfDir);
    for (const auto& e : fs::directory_iterator(wfDir)) {
        if (e.path().extension() == ".yml")
            fmt::print("  - {}\n", e.path().filename().string());
    }
    return 0;
}

} // namespace manager
