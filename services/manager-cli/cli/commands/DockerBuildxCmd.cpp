/// @file DockerBuildxCmd.cpp
#include "DockerBuildxCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>
#include <string>
#include <vector>

namespace manager
{

/// Check if a Docker image exists locally.
static bool imageExists(const std::string& image)
{
    auto cmd = fmt::format("docker image inspect {} >/dev/null 2>&1", image);
    return std::system(cmd.c_str()) == 0;
}

/// Build a single app image, wiring deps if available.
static int buildOne(const std::string& target, const std::string& registry,
                    const std::string& platform, bool noDeps)
{
    auto context = target;
    auto dockerfile = fmt::format("{}/Dockerfile", target);
    auto image = fmt::format("{}/{}", registry, target);
    auto sha = capture("git rev-parse --short HEAD 2>/dev/null");

    std::string cmd = "docker buildx build";
    cmd += fmt::format(" -f {}", dockerfile);
    cmd += fmt::format(" --platform {}", platform);
    cmd += fmt::format(" -t {}:latest", image);
    if (!sha.empty())
        cmd += fmt::format(" -t {}:sha-{}", image, sha);

    // Inject deps image if it exists locally.
    if (!noDeps) {
        auto deps = fmt::format("{}/{}-deps:latest", registry, target);
        if (imageExists(deps)) {
            cmd += fmt::format(" --build-arg DEPS_IMAGE={}", deps);
            fmt::print("[buildx] Using deps: {}\n", deps);
        } else {
            fmt::print("[buildx] No deps image found, "
                       "using debian:sid\n");
        }
    }

    cmd += " --load " + context;

    fmt::print("[buildx] {} -> {}\n", target, platform);
    return shell("buildx", cmd);
}

void DockerBuildxCmd::registerSub(CLI::App& docker)
{
    auto* sub = docker.add_subcommand("buildx", "Build app images with buildx");

    static std::string target = "all";
    static std::string platform;
    static bool noDeps = false;

    sub->add_option("-t,--target", target, "backend, frontend, or all");
    sub->add_option("-p,--platform", platform, "e.g. linux/arm64");
    sub->add_flag("--no-deps", noDeps, "Skip deps image injection");

    sub->callback([]() { execute(target, platform, noDeps); });
}

int DockerBuildxCmd::execute(const std::string& target,
                             const std::string& platform, bool noDeps)
{
    if (!checkDaemon())
        return 1;

    auto plat = platform.empty() ? hostPlatform() : platform;
    auto reg = detectRegistry();

    fmt::print("[buildx] {} @ {}\n", reg, plat);

    std::vector<std::string> targets;
    if (target == "all" || target == "backend")
        targets.emplace_back("backend");
    if (target == "all" || target == "frontend")
        targets.emplace_back("frontend");

    if (targets.empty()) {
        fmt::print("[buildx] Unknown: {}\n", target);
        return 1;
    }

    for (const auto& t : targets) {
        int rc = buildOne(t, reg, plat, noDeps);
        if (rc != 0)
            return rc;
    }

    fmt::print("[buildx] Done.\n");
    return 0;
}

} // namespace manager
