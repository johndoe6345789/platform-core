/**
 * @file S3CmdUp.cpp
 * @brief S3 server start (DB + app container).
 */

#include "S3Cmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

namespace manager
{

static int startS3Db()
{
    auto state =
        capture(fmt::format("docker inspect -f '{{{{.State.Running}}}}' {} "
                            "2>/dev/null",
                            S3Cmd::kDbContainer));
    if (state == "true")
        return 0;

    shell("s3", fmt::format("docker rm -f {} 2>/dev/null || true",
                            S3Cmd::kDbContainer));
    shell("s3", fmt::format("docker volume create {}", S3Cmd::kDbVol));

    return shell("s3", fmt::format("docker run -d --name {} --network {} "
                                   "--mount type=volume,src={},dst=/var/lib/"
                                   "postgresql/data "
                                   "-e POSTGRES_DB=s3server "
                                   "-e POSTGRES_USER=s3user "
                                   "-e POSTGRES_PASSWORD=s3pass "
                                   "postgres:16-alpine",
                                   S3Cmd::kDbContainer, S3Cmd::kNetwork,
                                   S3Cmd::kDbVol));
}

int S3Cmd::up()
{
    if (!checkDaemon())
        return 1;

    auto state =
        capture(fmt::format("docker inspect -f '{{{{.State.Running}}}}' {} "
                            "2>/dev/null",
                            kContainer));
    if (state == "true") {
        fmt::print("[s3] Already running\n");
        return 0;
    }

    shell("s3", fmt::format("docker rm -f {} 2>/dev/null || true", kContainer));
    shell("s3", fmt::format("docker network create {} 2>/dev/null || true",
                            kNetwork));
    shell("s3", fmt::format("docker volume create {}", kDataVol));

    if (startS3Db() != 0)
        return 1;

    int rc = shell("s3", fmt::format("docker run -d --name {} --network {} "
                                     "-p {}:9000 "
                                     "--mount type=volume,src={},dst=/data/s3 "
                                     "-e S3_DB_CONN='host={} port=5432 "
                                     "dbname=s3server user=s3user "
                                     "password=s3pass' "
                                     "-e S3_DATA_DIR=/data/s3 "
                                     "-e S3_REGION=us-east-1 "
                                     "-e S3_PORT=9000 "
                                     "-e PGHOST={} -e PGPORT=5432 "
                                     "-e PGUSER=s3user "
                                     "-e PGPASSWORD=s3pass "
                                     "-e PGDATABASE=s3server "
                                     "nextra-s3server",
                                     kContainer, kNetwork, kPort, kDataVol,
                                     kDbContainer, kDbContainer));

    if (rc == 0)
        fmt::print("[s3] Started on port {}\n", kPort);
    return rc;
}

} // namespace manager
