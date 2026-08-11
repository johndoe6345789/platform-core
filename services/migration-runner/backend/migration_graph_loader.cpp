/**
 * @file migration_graph_loader.cpp
 * @brief Implementation of loadMigrationGraph.
 */

#include "migration-runner/backend/migration_graph_loader.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fstream>
#include <stdexcept>

namespace services::migrations
{

DomainGraph loadMigrationGraph(const std::string& path)
{
    std::ifstream f(path);
    if (!f) {
        throw std::runtime_error(
            "migration-graph.json not found: " + path);
    }
    nlohmann::json j;
    try {
        f >> j;
    }
    catch (const std::exception& e) {
        throw std::runtime_error(
            std::string("migration-graph.json parse error: ") + e.what());
    }
    if (!j.is_object()) {
        throw std::runtime_error(
            "migration-graph.json root must be an object");
    }
    DomainGraph graph;
    for (auto it = j.begin(); it != j.end(); ++it) {
        const std::string domain = it.key();
        std::vector<std::string> deps;
        if (it.value().is_array()) {
            for (const auto& d : it.value()) {
                if (d.is_string()) {
                    deps.push_back(d.get<std::string>());
                }
            }
        }
        graph.emplace(domain, std::move(deps));
    }
    spdlog::debug("Loaded migration graph: {} domain(s)", graph.size());
    return graph;
}

} // namespace services::migrations
