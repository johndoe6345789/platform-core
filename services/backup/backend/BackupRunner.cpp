/**
 * @file BackupRunner.cpp
 * @brief Implements the dump -> encrypt -> upload orchestrator.
 */

#include "BackupRunner.h"

#include "AesGcmStream.h"
#include "PgDumpRunner.h"
#include "S3Uploader.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <ctime>
#include <sstream>
#include <stdexcept>

namespace nextra::backup
{

namespace
{
std::string makeKey(DumpKind k)
{
    const auto t = std::time(nullptr);
    char ts[32];
    std::strftime(ts, 32, "%Y%m%dT%H%M%SZ", std::gmtime(&t));
    std::ostringstream os;
    os << "backups/" << ts << "-" << kindToString(k) << ".pgd.enc";
    return os.str();
}
}  // namespace

std::int64_t BackupRunner::insertRunning(DumpKind kind)
{
    auto rows = db_->execSqlSync(
        "INSERT INTO backup_runs (status, kind) "
        "VALUES ('running', $1) RETURNING id",
        kindToString(kind));
    return rows[0]["id"].as<std::int64_t>();
}

void BackupRunner::finalizeSuccess(const BackupResult& r)
{
    db_->execSqlSync(
        "UPDATE backup_runs SET status='ok', finished_at=now(), "
        "bytes=$1, s3_key=$2 WHERE id=$3",
        r.bytes, r.s3Key, r.id);
}

void BackupRunner::finalizeFailure(const BackupResult& r)
{
    db_->execSqlSync(
        "UPDATE backup_runs SET status='error', "
        "finished_at=now(), error=$1 WHERE id=$2",
        r.error, r.id);
}

BackupResult BackupRunner::run(DumpKind kind)
{
    BackupResult res;
    res.kind = kind;
    res.id   = insertRunning(kind);
    try
    {
        const char* key = std::getenv(cfg_.kmsKeyEnv.c_str());
        if (!key || !*key)
            throw std::runtime_error("missing BACKUP_KMS_KEY");

        AesGcmStream enc(key);
        res.bytes = runPgDump(
            cfg_, kind,
            [&](const unsigned char* p, std::size_t n) {
                enc.update(p, n);
            });
        enc.finish();

        res.s3Key = makeKey(kind);
        if (!uploadToS3(cfg_, res.s3Key, enc.output()))
            throw std::runtime_error("s3 upload failed");

        res.status = "ok";
        finalizeSuccess(res);
        spdlog::info("backup: run {} ok ({} bytes)", res.id,
                     res.bytes);
    }
    catch (const std::exception& ex)
    {
        res.status = "error";
        res.error  = ex.what();
        finalizeFailure(res);
        spdlog::error("backup: run {} failed: {}", res.id, ex.what());
    }
    return res;
}

}  // namespace nextra::backup
