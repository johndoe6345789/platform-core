/**
 * @file JobRegistry.cpp
 * @brief Implementation of the thread-safe handler registry.
 */

#include "JobRegistry.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace nextra::jobs
{

JobRegistry& JobRegistry::instance()
{
    static JobRegistry s;
    return s;
}

void JobRegistry::reg(const std::string& name, JobHandler fn)
{
    std::lock_guard lock(mtx_);
    const auto [it, inserted] = handlers_.emplace(name, std::move(fn));
    if (!inserted)
    {
        throw std::logic_error("JobRegistry: handler already registered: " + name);
    }
    spdlog::info("jobs: registered handler '{}'", name);
}

std::optional<JobHandler> JobRegistry::resolve(const std::string& name) const
{
    std::lock_guard lock(mtx_);
    if (const auto it = handlers_.find(name); it != handlers_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

std::size_t JobRegistry::size() const
{
    std::lock_guard lock(mtx_);
    return handlers_.size();
}

void JobRegistry::clearForTesting()
{
    std::lock_guard lock(mtx_);
    handlers_.clear();
}

}  // namespace nextra::jobs
