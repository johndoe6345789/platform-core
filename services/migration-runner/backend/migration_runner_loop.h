#pragma once
/**
 * @file migration_runner_loop.h
 * @brief Async loop over topo-sorted domain list.
 *
 * For each domain: compute pending files, apply them.
 * Calls onSuccess with a JSON array of applied domain
 * names when all domains are processed.
 */

#include "migration-runner/backend/MigrationStateStore.h"

#include <memory>
#include <string>
#include <vector>

namespace services::migrations
{

/**
 * @brief Iterate domains in order, applying pending files.
 *
 * @param servicesRoot  Path to the `services/` directory.
 * @param domains       Topo-sorted domain names.
 * @param idx           Current position (start at 0).
 * @param applied       Accumulator for applied domain names.
 * @param onSuccess     Callback when all domains done.
 * @param onError       Error callback.
 */
void runDomainLoop(
    const std::string& servicesRoot,
    std::vector<std::string> domains,
    std::size_t idx,
    std::shared_ptr<json> applied,
    services::Callback onSuccess,
    services::ErrCallback onError);

} // namespace services::migrations
