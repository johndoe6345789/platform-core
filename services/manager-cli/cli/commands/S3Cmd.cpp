/**
 * @file S3Cmd.cpp
 * @brief S3 server lifecycle: build, down, status, logs.
 */

#include "S3Cmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <string>

namespace manager
{

int S3Cmd::build()
{
    if (!checkDaemon())
        return 1;
    fmt::print("[s3] Building S3 server image ...\n");
    return shell("s3", "docker build -t nextra-s3server "
                       "-f tools/s3server/backend/Dockerfile "
                       "tools/s3server");
}

int S3Cmd::down()
{
    if (!checkDaemon())
        return 1;
    fmt::print("[s3] Stopping stack ...\n");
    shell("s3", fmt::format("docker rm -f {}", kContainer));
    shell("s3", fmt::format("docker rm -f {}", kDbContainer));
    return 0;
}

int S3Cmd::status()
{
    if (!checkDaemon())
        return 1;
    for (auto c : {kContainer, kDbContainer}) {
        auto st =
            capture(fmt::format("docker inspect -f '{{{{.State.Status}}}}' {} "
                                "2>/dev/null",
                                c));
        fmt::print("[s3] {}: {}\n", c, st.empty() ? "not running" : st);
    }
    fmt::print("[s3] API: http://localhost:{}\n", kPort);
    return 0;
}

void S3Cmd::registerAll(CLI::App& parent)
{
    auto* cmd = parent.add_subcommand("s3", "Local S3-compatible server");
    cmd->require_subcommand(1);

    cmd->add_subcommand("build", "Build S3 image")->callback([]() { build(); });
    cmd->add_subcommand("up", "Start stack")->callback([]() { up(); });
    cmd->add_subcommand("down", "Stop stack")->callback([]() { down(); });
    cmd->add_subcommand("status", "Show status")->callback([]() { status(); });
    cmd->add_subcommand("logs", "Tail logs")->callback([]() {
        shell("s3", fmt::format("docker logs -f --tail=100 {}", kContainer));
    });
}

} // namespace manager
