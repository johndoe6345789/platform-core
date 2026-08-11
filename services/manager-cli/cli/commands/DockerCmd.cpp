/**
 * @file DockerCmd.cpp
 * @brief Implementation of the Docker command hub.
 */

#include "DockerCmd.h"
#include "DockerBuildxCmd.h"
#include "DockerDepsCmd.h"
#include "DockerStatusCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <string>
#include <unordered_map>

namespace manager
{

int DockerCmd::compose(const std::string& action)
{
    if (!checkDaemon()) {
        return 1;
    }

    static const std::unordered_map<std::string, std::string> cmds = {
        {"build", "docker compose build"},
        {"up", "docker compose up -d"},
        {"down", "docker compose down"},
        {"logs", "docker compose logs -f --tail=100"},
    };

    auto it = cmds.find(action);
    if (it == cmds.end()) {
        fmt::print("[docker] Unknown: '{}'\n", action);
        return 1;
    }

    fmt::print("[docker] Running '{}' ...\n", action);
    int rc = shell("docker", it->second);
    if (rc != 0) {
        fmt::print("[docker] '{}' failed (exit {})\n", action, rc);
    }
    return rc;
}

void DockerCmd::registerAll(CLI::App& parent)
{
    auto* docker = parent.add_subcommand("docker", "Docker operations");
    docker->require_subcommand(1);

    // --- compose shortcuts ---
    auto* bld = docker->add_subcommand("build", "Build compose images");
    bld->callback([]() { compose("build"); });

    auto* up = docker->add_subcommand("up", "Start services (detached)");
    up->callback([]() { compose("up"); });

    auto* dn = docker->add_subcommand("down", "Stop services");
    dn->callback([]() { compose("down"); });

    auto* lg = docker->add_subcommand("logs", "Tail service logs");
    lg->callback([]() { compose("logs"); });

    // --- smart commands ---
    DockerBuildxCmd::registerSub(*docker);
    DockerDepsCmd::registerSub(*docker);
    DockerStatusCmd::registerSub(*docker);
}

} // namespace manager
