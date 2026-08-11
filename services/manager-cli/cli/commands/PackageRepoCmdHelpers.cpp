/**
 * @file PackageRepoCmdHelpers.cpp
 * @brief DB startup and S3 helpers for package repo.
 */

#include "PackageRepoCmd.h"
#include "S3Cmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <string>

namespace manager
{

int PackageRepoCmd::startDb()
{
    auto state = capture(fmt::format("docker inspect -f "
                                     "'{{{{.State.Running}}}}' {} 2>/dev/null",
                                     kDbContainer));
    if (state == "true")
        return 0;

    shell("repo",
          fmt::format("docker rm -f {} 2>/dev/null || true", kDbContainer));
    shell("repo", fmt::format("docker volume create {}", kDbVol));

    return shell("repo", fmt::format("docker run -d --name {} --network {} "
                                     "--mount type=volume,src={},dst=/var/lib/"
                                     "postgresql/data "
                                     "-e POSTGRES_DB=packagerepo "
                                     "-e POSTGRES_USER=packagerepo "
                                     "-e POSTGRES_PASSWORD=packagerepo "
                                     "postgres:16-alpine",
                                     kDbContainer, kNetwork, kDbVol));
}

int PackageRepoCmd::ensureS3()
{
    S3Cmd::up();
    shell("repo", fmt::format("docker network connect {} {} "
                              "2>/dev/null || true",
                              kNetwork, S3Cmd::kContainer));
    return 0;
}

} // namespace manager
