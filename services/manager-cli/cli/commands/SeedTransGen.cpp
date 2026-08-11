/**
 * @file SeedTransGen.cpp
 * @brief Reads i18n JSON and collects rows for SQL output.
 */

#include "SeedTransCmd.h"

#include <nlohmann/json.hpp>

#include <fstream>
#include <iostream>
#include <vector>

namespace manager
{

using json = nlohmann::json;

/// Flatten nested JSON to dot-notation pairs.
static void flatten(
    const json& obj, const std::string& pfx,
    std::vector<std::pair<std::string,
                          std::string>>& kv)
{
    for (auto& [k, v] : obj.items()) {
        auto full = pfx.empty()
            ? k : pfx + "." + k;
        if (v.is_object())
            flatten(v, full, kv);
        else if (v.is_string())
            kv.emplace_back(
                full, v.get<std::string>());
    }
}

int SeedTransCmd::generate(
    const std::filesystem::path& msgDir,
    const std::string& outFile)
{
    if (!std::filesystem::exists(msgDir)) {
        std::cerr << "Directory not found: "
                  << msgDir.string() << "\n";
        return 1;
    }

    std::vector<SeedRow> rows;
    for (auto& e :
         std::filesystem::directory_iterator(msgDir))
    {
        if (e.path().extension() != ".json") continue;
        auto loc = e.path().stem().string();
        std::ifstream f(e.path());
        json data;
        try { data = json::parse(f); }
        catch (const json::exception& ex) {
            std::cerr << "Bad JSON " << loc
                      << ": " << ex.what() << "\n";
            continue;
        }
        for (auto& [ns, block] : data.items()) {
            if (!block.is_object()) continue;
            std::vector<std::pair<std::string,
                                  std::string>> kv;
            flatten(block, "", kv);
            for (auto& [k, v] : kv)
                rows.push_back({loc, ns, k, v});
        }
    }

    std::ostream* os = &std::cout;
    std::ofstream file;
    if (!outFile.empty()) {
        file.open(outFile);
        os = &file;
    }
    writeOutput(*os, rows);

    std::cerr << "[seed-trans] " << rows.size()
              << " rows from "
              << msgDir.string() << "\n";
    return 0;
}

} // namespace manager
