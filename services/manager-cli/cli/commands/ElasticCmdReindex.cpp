/**
 * @file ElasticCmdReindex.cpp
 * @brief Implementation of `es reindex` — deletes and
 *        recreates indices for a given service, then
 *        advises a service restart to re-populate.
 */

#include "ElasticCmd.h"
#include "ElasticCmdUtil.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <string>
#include <vector>

namespace manager
{

namespace
{

/// @brief Delete then recreate one index.
int resetIndex(const std::string& idx)
{
    fmt::print("[es] Deleting index '{}' ...\n", idx);
    shell("es", fmt::format(
        "curl -s -X DELETE {}/{}",
        ElasticCmd::kBaseUrl, idx));
    return es::createIndex(idx);
}

} // namespace

int ElasticCmd::reindex(const std::string& service)
{
    auto root = repoRoot();
    if (root.empty()) {
        fmt::print("[es] Cannot find repo root\n");
        return 1;
    }

    auto svcs = es::allServices();
    std::vector<es::SvcCfg> targets;

    if (service == "all") {
        targets = svcs;
    } else {
        bool found = false;
        for (auto& s : svcs) {
            if (s.name == service) {
                targets.push_back(s);
                found = true;
                break;
            }
        }
        if (!found) {
            fmt::print("[es] Unknown '{}'\n", service);
            fmt::print("[es] Valid: nextra, pkgrepo, "
                       "s3, all\n");
            return 1;
        }
    }

    int failures = 0;
    for (auto& svc : targets) {
        auto full = (root / svc.path).string();
        for (auto& idx : es::loadIndices(full)) {
            if (resetIndex(idx) != 0)
                ++failures;
        }
    }

    fmt::print("\n[es] Reindex done ({} failures)\n",
               failures);
    fmt::print("[es] Restart the service(s) to "
               "re-populate data.\n");
    return failures > 0 ? 1 : 0;
}

} // namespace manager
