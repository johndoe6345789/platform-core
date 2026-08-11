/**
 * @file migration_graph_loader.h
 * @brief Loads services/migration-graph.json into a
 *        plain std::map<domain, deps> structure.
 */
#pragma once

#include <map>
#include <string>
#include <vector>

namespace services::migrations
{

using DomainGraph = std::map<std::string, std::vector<std::string>>;

/**
 * @brief Read and parse the migration-graph.json file.
 * @param path Absolute or cwd-relative path to the JSON.
 * @return Map of domain name to list of dependency domain names.
 * @throws std::runtime_error on I/O or parse error.
 */
DomainGraph loadMigrationGraph(const std::string& path);

} // namespace services::migrations
