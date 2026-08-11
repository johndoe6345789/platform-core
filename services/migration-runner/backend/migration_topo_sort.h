/**
 * @file migration_topo_sort.h
 * @brief Kahn-style topological sort for the domain graph.
 */
#pragma once

#include "migration-runner/backend/migration_graph_loader.h"

#include <string>
#include <vector>

namespace services::migrations
{

/**
 * @brief Topologically sort domains so every dependency
 *        comes before its dependents.
 * @param graph Domain -> dep-list map.
 * @return Ordered list of domain names.
 * @throws std::runtime_error on unknown dep or cycle.
 */
std::vector<std::string> topoSortDomains(const DomainGraph& graph);

} // namespace services::migrations
