/**
 * @file MigrationRunner.cpp
 * @brief DAG-driven migration orchestrator implementation.
 */

#include "migration-runner/backend/MigrationRunner.h"
#include "migration-runner/backend/MigrationFileUtils.h"
#include "migration-runner/backend/MigrationStateStore.h"
#include "migration-runner/backend/migration_graph_loader.h"
#include "migration-runner/backend/migration_runner_bootstrap.h"
#include "migration-runner/backend/migration_runner_loop.h"
#include "migration-runner/backend/migration_topo_sort.h"

#include <spdlog/spdlog.h>

#include <memory>

namespace services
{

MigrationRunner::MigrationRunner(
    std::string servicesRoot,
    std::string graphPath,
    std::string bootstrapSql)
    : servicesRoot_(std::move(servicesRoot))
    , graphPath_(std::move(graphPath))
    , bootstrapSql_(std::move(bootstrapSql))
{
}

void MigrationRunner::runMigrations(
    Callback onSuccess, ErrCallback onError)
{
    std::string bsql;
    try {
        bsql = MigrationFileUtils::readFile(bootstrapSql_);
    } catch (const std::exception& e) {
        spdlog::error("Read bootstrap: {}", e.what());
        onError(drogon::k500InternalServerError,
                "Cannot read bootstrap SQL");
        return;
    }

    // Capture paths by value — the MigrationRunner
    // stack object may be destroyed before the async
    // DB callbacks fire.
    MigrationStateStore::ensureTable(
        [gp = graphPath_, sr = servicesRoot_,
         bsql, onSuccess, onError]() {
            migrations::MigrationRunnerBootstrap::run(
                bsql,
                [gp, sr, onSuccess, onError]() {
                    migrations::DomainGraph graph;
                    try {
                        graph = migrations::
                            loadMigrationGraph(gp);
                    } catch (const std::exception& e) {
                        spdlog::error("Load graph: {}",
                                      e.what());
                        onError(
                            drogon::k500InternalServerError,
                            "Cannot load migration graph");
                        return;
                    }
                    std::vector<std::string> order;
                    try {
                        order = migrations::topoSortDomains(
                            graph);
                    } catch (const std::exception& e) {
                        spdlog::error("Topo sort: {}",
                                      e.what());
                        onError(
                            drogon::k500InternalServerError,
                            "Migration graph cycle");
                        return;
                    }
                    auto applied =
                        std::make_shared<json>(
                            json::array());
                    migrations::runDomainLoop(
                        sr, order, 0,
                        applied, onSuccess, onError);
                },
                onError);
        },
        onError);
}

} // namespace services
