#pragma once

/**
 * @file PgDumpRunner.h
 * @brief Spawns pg_dump and streams stdout chunks to a sink.
 *
 * Uses fork/exec via popen for simplicity; the sink callback is
 * invoked for each 64KiB block and may encrypt/upload in-place.
 */

#include "BackupTypes.h"

#include <cstdint>
#include <functional>
#include <string>

namespace nextra::backup
{

/// Callback invoked for every chunk read from pg_dump stdout.
using ChunkSink =
    std::function<void(const unsigned char*, std::size_t)>;

/**
 * @brief Runs pg_dump against the configured DB.
 *
 * @param cfg   Backup configuration (dump binary + creds).
 * @param kind  Requested dump shape (full/schema/data).
 * @param sink  Called once per chunk; total bytes returned.
 * @return Total bytes read from pg_dump stdout.
 * @throws std::runtime_error if pg_dump fails to spawn or exits
 *         with a non-zero status code.
 */
std::int64_t runPgDump(
    const BackupConfig& cfg, DumpKind kind, ChunkSink sink);

}  // namespace nextra::backup
