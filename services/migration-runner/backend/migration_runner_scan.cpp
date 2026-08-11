/**
 * @file migration_runner_scan.cpp
 * @brief Pending-migration scanner implementation.
 */

#include "migration-runner/backend/migration_runner_scan.h"
#include "migration-runner/backend/MigrationFileUtils.h"
#include "migration-runner/backend/MigrationStateStore.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>

#include <algorithm>

namespace services::migrations
{

using namespace drogon;
using namespace drogon::orm;

void MigrationRunnerScan::pending(
    const std::string& servicesRoot,
    const std::string& domain,
    std::function<void(std::vector<std::string>)> onReady,
    services::ErrCallback onError)
{
    auto dir = fmt::format(
        "{}/{}/migrations", servicesRoot, domain);
    auto allFiles =
        services::MigrationFileUtils::discoverFiles(dir);

    auto db = services::MigrationStateStore::db();
    // Include 'legacy' rows: pre-domain-column
    // migrations were retro-stamped 'legacy' by the
    // bootstrap step and must not be re-applied.
    const std::string sql =
        "SELECT filename FROM schema_migrations "
        "WHERE domain=$1 OR domain='legacy' "
        "ORDER BY filename";

    *db << sql << domain
        >> [allFiles, onReady](const Result& r) {
            std::vector<std::string> applied;
            applied.reserve(r.size());
            for (const auto& row : r)
                applied.push_back(
                    row["filename"].as<std::string>());
            std::vector<std::string> out;
            for (const auto& f : allFiles) {
                if (std::ranges::find(applied, f)
                        == applied.end())
                    out.push_back(f);
            }
            onReady(std::move(out));
        }
        >> [domain, onError](
               const DrogonDbException& e) {
            spdlog::error("scan {}: {}",
                          domain, e.base().what());
            onError(k500InternalServerError,
                    fmt::format("Scan failed: {}",
                                domain));
        };
}

} // namespace services::migrations
