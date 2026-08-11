#pragma once

/**
 * @file BackupTypes.h
 * @brief Shared value types for the backup-manager daemon.
 *
 * All pg_dump / encrypt / S3 upload modules exchange data
 * through these POD structs so the orchestrator can wire
 * them without cross-dependencies.
 */

#include <cstdint>
#include <string>

namespace nextra::backup
{

/// Which shape of dump to capture.
enum class DumpKind
{
    Full,   ///< Schema + data (default nightly)
    Schema, ///< DDL only
    Data    ///< Data only
};

/// Runtime configuration loaded from backup-manager.json.
struct BackupConfig
{
    std::string bucket{"nextra-backups"};
    std::string kmsKeyEnv{"BACKUP_KMS_KEY"};
    std::string pgDumpPath{"/usr/bin/pg_dump"};
    std::string s3Endpoint{"http://s3:9000"};
    std::string s3AccessKey{"minioadmin"};
    std::string dbHost{"db"};
    int         dbPort{5432};
    std::string dbName{"nextra_db"};
    std::string dbUser{"nextra"};
    std::string dbPassword{"nextra_dev"};
};

/// Result of a single backup run, persisted to backup_runs.
struct BackupResult
{
    std::int64_t id{0};
    std::string  status{"running"};
    std::int64_t bytes{0};
    std::string  s3Key;
    std::string  error;
    DumpKind     kind{DumpKind::Full};
};

/// Convert a DumpKind to its SQL/enum text representation.
inline std::string kindToString(DumpKind k)
{
    switch (k)
    {
        case DumpKind::Schema: return "schema";
        case DumpKind::Data:   return "data";
        default:               return "full";
    }
}

}  // namespace nextra::backup
