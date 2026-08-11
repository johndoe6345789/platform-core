/**
 * @file ActCmd.h
 * @brief Run GitHub Actions workflows locally via act.
 */

#pragma once

#include <CLI/CLI.hpp>

#include <map>
#include <set>
#include <string>

namespace manager
{

/// @brief Runs .local/ workflows with nektos/act.
class ActCmd
{
  public:
    static constexpr const char* kWorkflowRel = ".local/workflows";
    static constexpr const char* kConfigRel = ".local/act-config.json";

    /// @brief Resolve the absolute workflow directory.
    static std::string workflowDir();

    /// @brief Load runner label -> image map from config.
    static std::map<std::string, std::string> loadRunners();

    /// @brief Extract unique runs-on labels from a workflow.
    static std::set<std::string> extractRunners(const std::string& path);

    /// @brief Map a runner label to a Docker image.
    static std::string
    runnerImage(const std::string& label,
                const std::map<std::string, std::string>& map);

    /// @brief Register all subcommands under "act".
    static void registerAll(CLI::App& parent);

    /// @brief Check if act is installed.
    static bool checkAct();

    /// @brief Run a specific workflow file.
    static int runWorkflow(const std::string& file, const std::string& job,
                           bool verbose);

    /// @brief List available workflows.
    static int list();
};

} // namespace manager
