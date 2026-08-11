#pragma once

/**
 * @file BackupRegistry.h
 * @brief Process-wide singleton holding the active BackupRunner.
 *
 * The backup-manager daemon registers its runner on startup so
 * that BackupsController (loaded into the same Drogon app) can
 * reach it without a hard link-time dependency.
 */

#include "BackupRunner.h"

#include <memory>
#include <mutex>

namespace nextra::backup
{

/// Process-wide accessor for the active backup runner instance.
class BackupRegistry
{
public:
    static BackupRegistry& instance()
    {
        static BackupRegistry reg;
        return reg;
    }

    /// Register the runner created by cmdBackupManager.
    void setRunner(std::shared_ptr<BackupRunner> r)
    {
        std::lock_guard<std::mutex> lk(m_);
        runner_ = std::move(r);
    }

    /// Fetch the runner or nullptr if the daemon is not live.
    std::shared_ptr<BackupRunner> runner() const
    {
        std::lock_guard<std::mutex> lk(m_);
        return runner_;
    }

private:
    BackupRegistry() = default;
    mutable std::mutex            m_;
    std::shared_ptr<BackupRunner> runner_;
};

}  // namespace nextra::backup

using BackupRegistry = nextra::backup::BackupRegistry;
