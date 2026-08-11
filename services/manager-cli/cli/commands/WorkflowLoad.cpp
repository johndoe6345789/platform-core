/**
 * @file WorkflowLoad.cpp
 * @brief Parse a JSON command definition into a CLI subcommand.
 */

#include "WorkflowEngine.h"

#include <map>
#include <memory>
#include <string>

namespace manager
{

void registerCmd(CLI::App* parent, const nlohmann::json& def)
{
    auto name = def["name"].get<std::string>();
    auto desc = def["description"].get<std::string>();
    auto* sub = parent->add_subcommand(name, desc);

    auto opts = std::make_shared<std::map<std::string, std::string>>();
    auto flags = std::make_shared<std::map<std::string, bool>>();

    if (def.contains("options")) {
        for (const auto& opt : def["options"]) {
            auto oname = opt["name"].get<std::string>();
            auto help = opt.value("help", "");
            auto dflt = opt.value("default", "");
            (*opts)[oname] = dflt;
            if (opt.value("flag", false)) {
                (*flags)[oname] = false;
                sub->add_flag("--" + oname, (*flags)[oname], help);
            } else {
                sub->add_option("--" + oname, (*opts)[oname], help);
            }
        }
    }

    auto steps = def["steps"];
    sub->callback([opts, flags, steps]() {
        WorkflowCtx ctx;
        ctx.loadBuiltins();
        for (const auto& [k, v] : *opts)
            ctx.set("opt." + k, v);
        for (const auto& [k, v] : *flags)
            ctx.set("opt." + k, v ? "true" : "false");
        executeWorkflow(steps, ctx);
    });
}

} // namespace manager
