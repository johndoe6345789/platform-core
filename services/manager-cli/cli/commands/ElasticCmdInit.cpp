/**
 * @file ElasticCmdInit.cpp
 * @brief Implementation of `es init` — creates all
 *        Elasticsearch indices for every service by
 *        reading their constants JSON files.
 */

#include "ElasticCmd.h"
#include "ElasticCmdUtil.h"
#include "ShellUtil.h"

#include <fmt/core.h>

namespace manager
{

int ElasticCmd::init()
{
    auto root = repoRoot();
    if (root.empty()) {
        fmt::print("[es] Cannot find repo root\n");
        return 1;
    }

    int failures = 0;
    for (auto& svc : es::allServices()) {
        auto full = (root / svc.path).string();
        auto idxs = es::loadIndices(full);
        fmt::print("[es] Service '{}': {} indices\n",
                   svc.name, idxs.size());
        for (auto& idx : idxs) {
            if (es::createIndex(idx) != 0)
                ++failures;
        }
    }

    fmt::print("\n[es] Init complete ({} failures)\n",
               failures);
    return failures > 0 ? 1 : 0;
}

} // namespace manager
