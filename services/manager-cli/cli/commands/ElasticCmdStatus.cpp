/**
 * @file ElasticCmdStatus.cpp
 * @brief Implementation of `es status` — queries the
 *        Elasticsearch cluster health endpoint.
 */

#include "ElasticCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <string>

namespace manager
{

int ElasticCmd::status()
{
    fmt::print("[es] Checking cluster health ...\n");

    std::string cmd = fmt::format(
        "curl -s {}/_cluster/health?pretty",
        kBaseUrl);

    int rc = shell("es", cmd);
    if (rc != 0) {
        fmt::print(
            "[es] Could not reach Elasticsearch at {}\n",
            kBaseUrl);
        return 1;
    }

    fmt::print("\n[es] Indices:\n");
    shell("es", fmt::format(
        "curl -s {}/_cat/indices?v", kBaseUrl));

    return 0;
}

} // namespace manager
