/**
 * @file MigrationStatusQuery.cpp
 * @brief MigrationStatusQuery implementation.
 */

#include "migration-runner/backend/MigrationStatusQuery.h"
#include "migration-runner/backend/MigrationFileUtils.h"
#include "migration-runner/backend/MigrationStateStore.h"

#include <spdlog/spdlog.h>

#include <unordered_map>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void MigrationStatusQuery::getStatus(const std::string& migrationsDir,
                                     Callback onSuccess, ErrCallback onError)
{
    auto allFiles = MigrationFileUtils::discoverFiles(migrationsDir);
    auto dbClient = MigrationStateStore::db();
    const std::string sql = "SELECT filename, applied_at "
                            "FROM schema_migrations "
                            "ORDER BY filename";

    *dbClient << sql >> [allFiles, onSuccess](const Result& result) {
        std::unordered_map<std::string, std::string> appliedMap;
        for (const auto& row : result) {
            appliedMap[row["filename"].as<std::string>()] =
                row["applied_at"].as<std::string>();
        }
        json statuses = json::array();
        for (const auto& f : allFiles) {
            auto it = appliedMap.find(f);
            if (it != appliedMap.end()) {
                statuses.push_back({{"filename", f},
                                    {"appliedAt", it->second},
                                    {"pending", false}});
            } else {
                statuses.push_back({{"filename", f},
                                    {"appliedAt", nullptr},
                                    {"pending", true}});
            }
        }
        onSuccess(statuses);
    } >> [onError](const DrogonDbException& e) {
        spdlog::error("getMigrationStatus error: {}", e.base().what());
        onError(k500InternalServerError, "Internal server error");
    };
}

} // namespace services
