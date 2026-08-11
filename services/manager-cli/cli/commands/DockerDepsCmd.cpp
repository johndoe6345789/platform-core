/// @file DockerDepsCmd.cpp - smart deps image builder.

#include "DockerDepsCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>
#include <string>
#include <vector>

namespace manager
{

/// Build a single deps image for the given target.
static int buildOne(const std::string& target, const std::string& registry,
                    const std::string& platform, bool push)
{
    auto dockerfile = fmt::format("docker/{}-deps.Dockerfile", target);
    auto image = fmt::format("{}/{}-deps", registry, target);
    auto sha = capture("git rev-parse --short HEAD 2>/dev/null");

    bool multi = platform.find(',') != std::string::npos;
    if (multi && !push) {
        fmt::print("[deps] Multi-platform builds "
                   "require --push\n");
        return 1;
    }

    std::string cmd = "docker buildx build";
    cmd += fmt::format(" -f {}", dockerfile);
    cmd += fmt::format(" --platform {}", platform);
    cmd += fmt::format(" -t {}:latest", image);
    if (!sha.empty())
        cmd += fmt::format(" -t {}:sha-{}", image, sha);
    cmd += push ? " --push" : " --load";
    cmd += " .";

    fmt::print("[deps] {} -> {}\n", target, platform);
    return shell("deps", cmd);
}

void DockerDepsCmd::registerSub(CLI::App& docker)
{
    auto* sub = docker.add_subcommand("deps", "Build base dependency images");

    static std::string target = "all";
    static std::string platform;
    static bool push = false;

    sub->add_option("-t,--target", target, "backend, frontend, or all");
    sub->add_option("-p,--platform", platform, "e.g. linux/amd64,linux/arm64");
    sub->add_flag("--push", push, "Push to registry");

    sub->callback([]() { execute(target, platform, push); });
}

int DockerDepsCmd::execute(const std::string& target,
                           const std::string& platform, bool push)
{
    if (!checkDaemon())
        return 1;

    auto plat = platform.empty() ? hostPlatform() : platform;
    auto reg = detectRegistry();

    fmt::print("[deps] {} @ {}\n", reg, plat);

    std::vector<std::string> targets;
    if (target == "all" || target == "backend")
        targets.emplace_back("backend");
    if (target == "all" || target == "frontend")
        targets.emplace_back("frontend");

    if (targets.empty()) {
        fmt::print("[deps] Unknown: {}\n", target);
        return 1;
    }

    for (const auto& t : targets) {
        int rc = buildOne(t, reg, plat, push);
        if (rc != 0)
            return rc;
    }

    fmt::print("[deps] Done.\n");
    return 0;
}

} // namespace manager
