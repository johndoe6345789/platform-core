/**
 * @file PgDumpRunner.cpp
 * @brief Implementation of the pg_dump streaming runner.
 */

#include "PgDumpRunner.h"

#include <spdlog/spdlog.h>

#include <array>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>

namespace nextra::backup
{

namespace
{
std::string buildCmd(const BackupConfig& c, DumpKind k)
{
    std::ostringstream os;
    os << "PGPASSWORD='" << c.dbPassword << "' "
       << c.pgDumpPath
       << " -h " << c.dbHost
       << " -p " << c.dbPort
       << " -U " << c.dbUser
       << " -d " << c.dbName;
    if (k == DumpKind::Schema) os << " --schema-only";
    else if (k == DumpKind::Data) os << " --data-only";
    os << " -Fc 2>/dev/null";
    return os.str();
}
}  // namespace

std::int64_t runPgDump(
    const BackupConfig& cfg, DumpKind kind, ChunkSink sink)
{
    const std::string cmd = buildCmd(cfg, kind);
    spdlog::debug("backup: spawn pg_dump ({})", kindToString(kind));

    FILE* pipe = ::popen(cmd.c_str(), "r");
    if (!pipe)
    {
        throw std::runtime_error("pg_dump: popen failed");
    }

    std::array<unsigned char, 64 * 1024> buf{};
    std::int64_t total = 0;
    while (true)
    {
        const std::size_t n =
            std::fread(buf.data(), 1, buf.size(), pipe);
        if (n > 0)
        {
            sink(buf.data(), n);
            total += static_cast<std::int64_t>(n);
        }
        if (n < buf.size()) break;
    }

    const int rc = ::pclose(pipe);
    if (rc != 0)
    {
        throw std::runtime_error(
            "pg_dump: exit status " + std::to_string(rc));
    }
    spdlog::info("backup: pg_dump produced {} bytes", total);
    return total;
}

}  // namespace nextra::backup
