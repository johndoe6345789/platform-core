/**
 * @file backup_manager.cpp
 * @brief CLI subcommand implementation for the backup daemon.
 */

#include "backup/backend/commands/backup_manager.h"

#include "backup/backend/BackupRunner.h"
#include "backup/backend/BackupTypes.h"
#include "backup/backend/BackupRegistry.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <thread>

namespace
{
std::atomic<bool> g_stop{false};

void onSignal(int) { g_stop.store(true); }

nextra::backup::BackupConfig loadConfig()
{
    using namespace nextra::backup;
    BackupConfig c;
    std::ifstream f("constants/backup-manager.json");
    if (f)
    {
        nlohmann::json j;
        f >> j;
        c.bucket      = j.value("bucket", c.bucket);
        c.kmsKeyEnv   = j.value("kmsKeyEnv", c.kmsKeyEnv);
        c.pgDumpPath  = j.value("pgDumpPath", c.pgDumpPath);
        c.s3Endpoint  = j.value("s3Endpoint", c.s3Endpoint);
    }
    if (auto* h = std::getenv("DB_HOST"))     c.dbHost     = h;
    if (auto* u = std::getenv("DB_USER"))     c.dbUser     = u;
    if (auto* p = std::getenv("DB_PASSWORD")) c.dbPassword = p;
    if (auto* n = std::getenv("DB_NAME"))     c.dbName     = n;
    return c;
}
}  // namespace

namespace commands
{

void cmdBackupManager(const std::string& config)
{
    using namespace nextra::backup;
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    drogon::app().loadConfigFile(config);
    auto db  = drogon::app().getDbClient();
    auto cfg = loadConfig();

    auto runner = std::make_shared<BackupRunner>(db, cfg);
    BackupRegistry::instance().setRunner(runner);
    spdlog::info("backup-manager daemon ready");

    std::thread httpThread([] { drogon::app().run(); });
    while (!g_stop.load())
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

    spdlog::info("backup-manager: shutdown signal received");
    drogon::app().quit();
    if (httpThread.joinable()) httpThread.join();
}

}  // namespace commands
