/**
 * @file migration_runner_loop.cpp
 * @brief Async domain-iteration loop implementation.
 */

#include "migration-runner/backend/migration_runner_loop.h"
#include "migration-runner/backend/migration_runner_domain.h"
#include "migration-runner/backend/migration_runner_scan.h"

#include <spdlog/spdlog.h>

namespace services::migrations
{

void runDomainLoop(
    const std::string& servicesRoot,
    std::vector<std::string> domains,
    std::size_t idx,
    std::shared_ptr<json> applied,
    services::Callback onSuccess,
    services::ErrCallback onError)
{
    if (idx >= domains.size()) {
        onSuccess(*applied);
        return;
    }
    const auto& domain = domains[idx];
    MigrationRunnerScan::pending(
        servicesRoot, domain,
        [servicesRoot, domains, idx, domain,
         applied, onSuccess, onError](
            std::vector<std::string> pending) {
            if (pending.empty()) {
                spdlog::debug(
                    "No pending for {}", domain);
                runDomainLoop(servicesRoot, domains,
                              idx + 1, applied,
                              onSuccess, onError);
                return;
            }
            MigrationRunnerDomain::applyAll(
                servicesRoot, domain,
                std::move(pending),
                [servicesRoot, domains, idx, domain,
                 applied, onSuccess, onError]() {
                    applied->push_back(domain);
                    runDomainLoop(
                        servicesRoot, domains,
                        idx + 1, applied,
                        onSuccess, onError);
                },
                onError);
        },
        onError);
}

} // namespace services::migrations
