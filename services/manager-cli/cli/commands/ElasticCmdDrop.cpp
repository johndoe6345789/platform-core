/**
 * @file ElasticCmdDrop.cpp
 * @brief Implementation of `es drop` — deletes a single
 *        Elasticsearch index after confirmation.
 */

#include "ElasticCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <iostream>
#include <string>

namespace manager
{

int ElasticCmd::drop(const std::string& index)
{
    fmt::print(
        "[es] About to DELETE index '{}'\n", index);
    fmt::print("[es] This cannot be undone. Continue? "
               "[y/N] ");

    std::string answer;
    std::getline(std::cin, answer);
    if (answer != "y" && answer != "Y") {
        fmt::print("[es] Aborted.\n");
        return 0;
    }

    fmt::print("[es] Deleting index '{}' ...\n", index);
    std::string cmd = fmt::format(
        "curl -s -X DELETE {}/{}?pretty",
        kBaseUrl, index);

    int rc = shell("es", cmd);
    if (rc != 0) {
        fmt::print("[es] Failed to delete '{}'\n", index);
        return 1;
    }

    fmt::print("\n[es] Index '{}' deleted.\n", index);
    return 0;
}

} // namespace manager
