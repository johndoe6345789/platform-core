/**
 * @file ElasticCmdUtil.h
 * @brief Shared helpers for Elasticsearch commands:
 *        service config paths, index loading, and
 *        index creation via curl.
 */

#pragma once

#include "ElasticCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace manager::es
{

/// @brief ES settings body for index creation.
inline constexpr const char* kSettings =
    R"({"settings":{"number_of_shards":1,)"
    R"("number_of_replicas":0,"refresh_interval":"1s"}})";

/// @brief Service name to config path mapping.
struct SvcCfg {
    std::string name;
    std::string path;
};

/// @brief All known service configurations.
inline std::vector<SvcCfg> allServices()
{
    return {
        {"nextra",
         "backend/src/constants/elasticsearch.json"},
        {"pkgrepo",
         "tools/packagerepo/backend/src/"
         "constants/elasticsearch.json"},
        {"s3",
         "tools/s3server/backend/src/"
         "constants/elasticsearch.json"},
    };
}

/// @brief Read index names from an ES config file.
/// @param path Path to the elasticsearch.json file.
/// @return Vector of index names.
inline std::vector<std::string> loadIndices(
    const std::string& path)
{
    std::vector<std::string> out;
    std::ifstream f(path);
    if (!f.is_open()) {
        fmt::print("[es] Warning: cannot open {}\n", path);
        return out;
    }
    auto cfg = nlohmann::json::parse(f);
    if (!cfg.contains("indices"))
        return out;
    for (auto& [k, v] : cfg["indices"].items())
        out.push_back(v.get<std::string>());
    return out;
}

/// @brief Create a single ES index via curl PUT.
/// @param index The index name.
/// @return 0 on success.
inline int createIndex(const std::string& index)
{
    fmt::print("[es] Creating index '{}' ...\n", index);
    return shell("es", fmt::format(
        "curl -s -X PUT {}/{} "
        "-H 'Content-Type: application/json' -d '{}'",
        ElasticCmd::kBaseUrl, index, kSettings));
}

} // namespace manager::es
