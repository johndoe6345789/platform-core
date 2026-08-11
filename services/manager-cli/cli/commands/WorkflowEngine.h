/**
 * @file WorkflowEngine.h
 * @brief Data-driven workflow engine for JSON commands.
 *
 * Commands are defined in .local/commands/. The engine
 * parses steps and executes them with variable interpolation,
 * similar to n8n or Quake 3's data-driven architecture.
 */

#pragma once

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include <map>
#include <string>

namespace manager
{

/// @brief Variable context for workflow execution.
class WorkflowCtx
{
  public:
    /// @brief Set a variable.
    void set(const std::string& key, const std::string& val);

    /// @brief Get a variable (empty if missing).
    std::string get(const std::string& key) const;

    /// @brief Interpolate {var} placeholders in a string.
    std::string expand(const std::string& tmpl) const;

    /// @brief Populate built-in variables.
    void loadBuiltins();

  private:
    std::map<std::string, std::string> vars_;
};

/// @brief Execute a single workflow step.
/// @return 0 on success, non-zero on failure.
int executeStep(const nlohmann::json& step, WorkflowCtx& ctx);

/// @brief Execute all steps in a workflow.
/// @return 0 if all pass, first non-zero on failure.
int executeWorkflow(const nlohmann::json& steps, WorkflowCtx& ctx);

/// @brief Execute the "write" step type.
int stepWrite(const nlohmann::json& step, WorkflowCtx& ctx);

/// @brief Execute the "retry" step type.
int stepRetry(const nlohmann::json& step, WorkflowCtx& ctx);

/// @brief Execute the "template" step type.
int stepTemplate(const nlohmann::json& step, WorkflowCtx& ctx);

/// @brief Execute the "foreach" step type.
int stepForeach(const nlohmann::json& step, WorkflowCtx& ctx);

/// @brief Execute cmake-gen template rendering.
int executeCmakeGen(const std::string& input, const std::string& output,
                    const std::string& templates);

/// @brief Register one JSON command definition on a CLI app.
void registerCmd(CLI::App* parent, const nlohmann::json& def);

/// @brief Register JSON-defined commands on a CLI app.
void registerWorkflows(CLI::App& app);

} // namespace manager
