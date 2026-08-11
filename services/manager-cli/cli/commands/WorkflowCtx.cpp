/**
 * @file WorkflowCtx.cpp
 * @brief Variable context with built-in resolution.
 */

#include "ShellUtil.h"
#include "WorkflowEngine.h"

#include <fmt/core.h>

namespace manager
{

void WorkflowCtx::set(const std::string& key, const std::string& val)
{
    vars_[key] = val;
}

std::string WorkflowCtx::get(const std::string& key) const
{
    auto it = vars_.find(key);
    return it != vars_.end() ? it->second : "";
}

std::string WorkflowCtx::expand(const std::string& tmpl) const
{
    std::string result;
    size_t i = 0;
    while (i < tmpl.size()) {
        if (tmpl[i] == '{') {
            auto end = tmpl.find('}', i);
            if (end != std::string::npos) {
                auto key = tmpl.substr(i + 1, end - i - 1);
                result += get(key);
                i = end + 1;
                continue;
            }
        }
        result += tmpl[i++];
    }
    return result;
}

void WorkflowCtx::loadBuiltins()
{
    auto platform = hostPlatform();
    set("host_platform", platform);

    auto slash = platform.rfind('/');
    if (slash != std::string::npos)
        set("host_arch", platform.substr(slash + 1));

    auto root = repoRoot();
    if (!root.empty())
        set("repo_root", root.string());

    set("registry", detectRegistry());

    auto sha = capture("git rev-parse --short HEAD");
    if (!sha.empty())
        set("sha", sha);
}

} // namespace manager
