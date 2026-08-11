/**
 * @file PackageRepoCmdReg.cpp
 * @brief Package repository down, status, registerAll.
 */

#include "PackageRepoCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

namespace manager
{

int PackageRepoCmd::down()
{
    if (!checkDaemon())
        return 1;
    fmt::print("[repo] Stopping stack ...\n");
    shell("repo", fmt::format("docker rm -f {}", kContainer));
    shell("repo", fmt::format("docker rm -f {}", kDbContainer));
    return 0;
}

int PackageRepoCmd::status()
{
    if (!checkDaemon())
        return 1;
    for (auto c : {kContainer, kDbContainer}) {
        auto st =
            capture(fmt::format("docker inspect -f '{{{{.State.Status}}}}' {} "
                                "2>/dev/null",
                                c));
        fmt::print("[repo] {}: {}\n", c, st.empty() ? "not running" : st);
    }
    fmt::print("[repo] API: http://localhost:{}\n", kPort);
    return 0;
}

void PackageRepoCmd::registerAll(CLI::App& parent)
{
    auto* cmd = parent.add_subcommand("repo", "Local package repository");
    cmd->require_subcommand(1);

    cmd->add_subcommand("build", "Build backend image")->callback([]() {
        build();
    });
    cmd->add_subcommand("up", "Start stack")->callback([]() { up(); });
    cmd->add_subcommand("down", "Stop stack")->callback([]() { down(); });
    cmd->add_subcommand("status", "Show status")->callback([]() { status(); });
    cmd->add_subcommand("logs", "Tail logs")->callback([]() {
        shell("repo", fmt::format("docker logs -f --tail=100 {}", kContainer));
    });

    auto* pullCmd = cmd->add_subcommand("pull", "Pull a package");
    static std::string pullSpec;
    static std::string pullOut = ".";
    pullCmd->add_option("spec", pullSpec, "namespace/name@version")
        ->required();
    pullCmd->add_option("-o,--output", pullOut, "Output directory");
    pullCmd->callback([]() { pull(pullSpec, pullOut); });
}

} // namespace manager
