/**
 * @file ActCmd.cpp
 * @brief Core act logic: runner detection, workflow execution.
 */

#include "ActCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <regex>

namespace fs = std::filesystem;

namespace manager
{

bool ActCmd::checkAct()
{
    if (std::system("act --version >/dev/null 2>&1") != 0) {
        fmt::print("[act] 'act' is not installed.\n");
        fmt::print("[act] Install: brew install act\n");
        return false;
    }
    return true;
}

std::string ActCmd::workflowDir()
{
    auto root = repoRoot();
    if (root.empty())
        return "";
    return (root / kWorkflowRel).string();
}

std::map<std::string, std::string> ActCmd::loadRunners()
{
    std::map<std::string, std::string> m;
    auto root = repoRoot();
    if (root.empty())
        return m;
    std::ifstream in(root / kConfigRel);
    if (!in.is_open())
        return m;
    try {
        auto j = nlohmann::json::parse(in);
        if (j.contains("runners"))
            for (auto& [k, v] : j["runners"].items())
                m[k] = v.get<std::string>();
    } catch (const nlohmann::json::exception& e) {
        fmt::print("[act] Warning: bad {}: {}\n",
                   kConfigRel, e.what());
    }
    return m;
}

std::set<std::string> ActCmd::extractRunners(const std::string& path)
{
    std::set<std::string> labels;
    std::ifstream in(path);
    if (!in.is_open())
        return labels;
    std::regex re(R"(^\s*runs-on:\s*(\S+))");
    std::string line;
    while (std::getline(in, line)) {
        std::smatch match;
        if (std::regex_search(line, match, re))
            labels.insert(match[1].str());
    }
    return labels;
}

std::string ActCmd::runnerImage(const std::string& label,
                                const std::map<std::string, std::string>& map)
{
    if (auto it = map.find(label); it != map.end())
        return it->second;
    if (label.find("24.04") != std::string::npos)
        return "catthehacker/ubuntu:act-24.04";
    if (label.find("22.04") != std::string::npos)
        return "catthehacker/ubuntu:act-22.04";
    return "catthehacker/ubuntu:act-latest";
}

} // namespace manager
