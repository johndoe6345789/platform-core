/**
 * @file PackageRepoCmd.cpp
 * @brief Package repository build and up commands.
 */

#include "PackageRepoCmd.h"
#include "S3Cmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <string>

namespace manager
{

int PackageRepoCmd::build()
{
    if (!checkDaemon())
        return 1;
    fmt::print("[repo] Building backend image ...\n");
    return shell("repo", "docker build -t nextra-packagerepo "
                         "-f tools/packagerepo/backend/Dockerfile "
                         "tools/packagerepo");
}

int PackageRepoCmd::up()
{
    if (!checkDaemon())
        return 1;

    auto state = capture(fmt::format("docker inspect -f "
                                     "'{{{{.State.Running}}}}' {} 2>/dev/null",
                                     kContainer));
    if (state == "true") {
        fmt::print("[repo] Already running\n");
        return 0;
    }

    shell("repo",
          fmt::format("docker rm -f {} 2>/dev/null || true", kContainer));
    shell("repo", fmt::format("docker network create {} "
                              "2>/dev/null || true",
                              kNetwork));

    if (startDb() != 0)
        return 1;
    ensureS3();

    int rc = shell("repo",
                   fmt::format("docker run -d --name {} --network {} "
                               "-p {}:5000 "
                               "-e S3_ENDPOINT=http://{}:9000 "
                               "-e S3_BUCKET=packagerepo "
                               "-e S3_ACCESS_KEY=minioadmin "
                               "-e DATABASE_URL='host={} port=5432 "
                               "dbname=packagerepo user=packagerepo "
                               "password=packagerepo' "
                               "-e PGHOST={} -e PGPORT=5432 "
                               "-e PGUSER=packagerepo "
                               "-e PGPASSWORD=packagerepo "
                               "-e PGDATABASE=packagerepo "
                               "-e JWT_SECRET=dev-secret-key "
                               "nextra-packagerepo",
                               kContainer, kNetwork, kPort, S3Cmd::kContainer,
                               kDbContainer, kDbContainer));

    if (rc == 0)
        fmt::print("[repo] Started on port {}\n", kPort);
    return rc;
}

} // namespace manager
