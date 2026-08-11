/// @file WorkflowExec.cpp -- step executor (syscall layer).
#include "ShellUtil.h"
#include "WorkflowEngine.h"
#include <fmt/core.h>

namespace manager
{
static bool evalWhen(
    const std::string& c, const WorkflowCtx& ctx)
{
    if (c.empty())
        return true;
    auto x = ctx.expand(c);
    return !x.empty() && x != "false" && x != "0";
}
static int stepCheck(
    const nlohmann::json& s, WorkflowCtx& ctx)
{
    auto cmd = ctx.expand(s["command"].get<std::string>());
    if (std::system((cmd + " >/dev/null 2>&1").c_str())) {
        fmt::print("[workflow] {}\n",
                   ctx.expand(s.value("error",
                                      cmd + " failed")));
        return 1;
    }
    return 0;
}
static int stepShell(
    const nlohmann::json& s, WorkflowCtx& ctx)
{
    return shell(
        s.value("tag", "workflow"),
        ctx.expand(s["command"].get<std::string>()));
}
static int stepCapture(
    const nlohmann::json& s, WorkflowCtx& ctx)
{
    ctx.set(s["var"].get<std::string>(),
            capture(ctx.expand(
                s["command"].get<std::string>())));
    return 0;
}
static int stepSet(
    const nlohmann::json& s, WorkflowCtx& ctx)
{
    auto var = s["var"].get<std::string>();
    auto val = ctx.expand(s["value"].get<std::string>());
    if (val.empty() && s.contains("default"))
        val = ctx.expand(s["default"].get<std::string>());
    ctx.set(var, val);
    return 0;
}
static int stepPrint(
    const nlohmann::json& s, WorkflowCtx& ctx)
{
    fmt::print("{}\n",
               ctx.expand(s["text"].get<std::string>()));
    return 0;
}
int executeStep(
    const nlohmann::json& s, WorkflowCtx& ctx)
{
    if (s.contains("when") &&
        !evalWhen(s["when"].get<std::string>(), ctx))
        return 0;
    auto t = s["type"].get<std::string>();
    if (t == "check")   return stepCheck(s, ctx);
    if (t == "shell")   return stepShell(s, ctx);
    if (t == "capture") return stepCapture(s, ctx);
    if (t == "set")     return stepSet(s, ctx);
    if (t == "print")   return stepPrint(s, ctx);
    if (t == "foreach") return stepForeach(s, ctx);
    if (t == "write")   return stepWrite(s, ctx);
    if (t == "retry")   return stepRetry(s, ctx);
    if (t == "template") return stepTemplate(s, ctx);
    fmt::print("[workflow] Unknown step type: {}\n", t);
    return 1;
}
int executeWorkflow(
    const nlohmann::json& steps, WorkflowCtx& ctx)
{
    for (const auto& step : steps) {
        if (int rc = executeStep(step, ctx); rc)
            return rc;
    }
    return 0;
}
} // namespace manager
