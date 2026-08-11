/**
 * @file DockerStatusCmd.cpp
 * @brief Implementation of the Docker status dashboard.
 */

#include "DockerStatusCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <string>

namespace manager
{

static void section(const std::string& title)
{
    fmt::print("\n[status] --- {} ---\n", title);
}

/// Show Docker engine version and buildx availability.
static void showEngine()
{
    section("Engine");
    auto ver = capture("docker version --format "
                       "'{{.Server.Version}}' 2>/dev/null");
    if (ver.empty()) {
        fmt::print("[status] Docker: NOT RUNNING\n");
        return;
    }
    fmt::print("[status] Docker  : v{}\n", ver);

    auto bx = capture("docker buildx version 2>/dev/null");
    fmt::print("[status] Buildx  : {}\n", bx.empty() ? "NOT INSTALLED" : bx);

    auto arch = capture("uname -m");
    fmt::print("[status] Host    : {}\n", arch);
}

/// List project-related Docker images.
static void showImages()
{
    section("Images");
    auto out = capture("docker images --format "
                       "'  {{.Repository}}:{{.Tag}}"
                       "\t{{.Size}}\t{{.CreatedSince}}' "
                       "2>/dev/null | grep -E "
                       "'(deps|nextra|frontend|backend)' "
                       "| head -20");
    fmt::print("{}\n", out.empty() ? "  (none found)" : out);
}

/// List running containers from compose.
static void showContainers()
{
    section("Containers");
    auto out = capture("docker compose ps --format "
                       "'  {{.Name}}\t{{.Status}}\t{{.Ports}}' "
                       "2>/dev/null");
    fmt::print("{}\n", out.empty() ? "  (none running)" : out);
}

/// Show available buildx builders and platforms.
static void showBuilders()
{
    section("Builders");
    auto out = capture("docker buildx ls 2>/dev/null | head -10");
    fmt::print("{}\n", out.empty() ? "  (no builders)" : out);
}

void DockerStatusCmd::registerSub(CLI::App& docker)
{
    auto* sub = docker.add_subcommand("status", "Show Docker health dashboard");
    sub->callback([]() { execute(); });
}

int DockerStatusCmd::execute()
{
    showEngine();
    showImages();
    showContainers();
    showBuilders();
    fmt::print("\n");
    return 0;
}

} // namespace manager
