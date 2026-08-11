/**
 * @file ActCmdReg.cpp
 * @brief Data-driven CLI registration from act-config.json.
 */

#include "ActCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>
#include <nlohmann/json.hpp>

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace manager
{

/// @brief Expand {arch} in a workflow template string.
static std::string expandArch(const std::string& tmpl, const std::string& arch)
{
    auto pos = tmpl.find("{arch}");
    if (pos == std::string::npos)
        return tmpl;
    return tmpl.substr(0, pos) + arch + tmpl.substr(pos + 6);
}

/// @brief Register one JSON command entry as a subcommand.
static void registerEntry(CLI::App* parent, const json& entry)
{
    auto name = entry["name"].get<std::string>();
    auto desc = entry["description"].get<std::string>();
    auto tmpl = entry["workflow"].get<std::string>();
    bool arch = tmpl.find("{arch}") != std::string::npos;

    auto* sub = parent->add_subcommand(name, desc);
    auto st = std::make_shared<std::pair<std::string, bool>>("", false);
    if (arch)
        sub->add_option("--arch", st->first, "Target arch (default: host)");
    sub->add_flag("-v,--verbose", st->second, "Verbose");

    sub->callback([st, tmpl, arch]() {
        std::string wf = tmpl;
        if (arch) {
            auto plat =
                st->first.empty() ? hostPlatform() : "linux/" + st->first;
            auto s = plat.rfind('/');
            wf = expandArch(tmpl, plat.substr(s + 1));
            fmt::print("[act] {} -> {}\n", plat, wf);
        }
        if (int rc = ActCmd::runWorkflow(wf, "", st->second); rc)
            std::exit(rc);
    });
}

void ActCmd::registerAll(CLI::App& parent)
{
    auto* cmd = parent.add_subcommand("act", "Run .local workflows with act");

    cmd->add_subcommand("list", "List workflows")->callback([]() { list(); });

    auto root = repoRoot();
    if (!root.empty()) {
        std::ifstream in(root / kConfigRel);
        if (in.is_open()) {
            try {
                auto j = json::parse(in);
                if (j.contains("commands"))
                    for (const auto& e : j["commands"])
                        registerEntry(cmd, e);
            } catch (const json::exception& e) {
                fmt::print("[act] Warning: bad {}: {}\n",
                           kConfigRel, e.what());
            }
        }
    }

    auto* run = cmd->add_subcommand("run", "Run a specific workflow file");
    static std::string file, job;
    static bool verbose = false;
    run->add_option("file", file, "Workflow .yml")->required();
    run->add_option("-j,--job", job, "Run only this job");
    run->add_flag("-v,--verbose", verbose, "Verbose");
    run->callback([]() { runWorkflow(file, job, verbose); });
}

} // namespace manager
