#pragma once

/**
 * @file BackupRunner.h
 * @brief Orchestrates a full dump -> encrypt -> upload run.
 *
 * The runner owns the lifecycle of one backup_runs row: it
 * inserts the row at "running", streams pg_dump through the
 * GCM encryptor into a buffer, hands the buffer to the S3
 * uploader, then updates the row with the final status.
 */

#include "BackupTypes.h"

#include <drogon/orm/DbClient.h>

#include <memory>

namespace nextra::backup
{

/**
 * @brief Top-level orchestrator for a single backup run.
 */
class BackupRunner
{
public:
    BackupRunner(drogon::orm::DbClientPtr db, BackupConfig cfg)
        : db_{std::move(db)}, cfg_{std::move(cfg)}
    {
    }

    /**
     * @brief Execute a backup end-to-end and record the result.
     * @param kind Dump shape (full/schema/data).
     * @return Populated BackupResult for the persisted row.
     */
    BackupResult run(DumpKind kind);

private:
    std::int64_t insertRunning(DumpKind kind);
    void finalizeSuccess(const BackupResult& r);
    void finalizeFailure(const BackupResult& r);

    drogon::orm::DbClientPtr db_;
    BackupConfig             cfg_;
};

}  // namespace nextra::backup
