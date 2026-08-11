/**
 * @file cron_manager.cpp
 * @brief CLI subcommand implementation for the cron-manager daemon.
 */

#include "cron/backend/commands/cron_manager.h"

#include "cron/backend/CronManager.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>
#include <fstream>
#include <future>
#include <thread>

namespace nextra::cron
{
/// Set by cmdCronManager so CronController::forceTick can reach
/// the live CronManager instance inside this process.  Controllers
/// live in shared TUs, so a module-local global is the cleanest
/// seam — the alternative would be an app-wide service locator.
CronManager* g_cronManager = nullptr;
}  // namespace nextra::cron

namespace
{
std::atomic<bool> g_stop{false};
void onSignal(int) { g_stop.store(true); }

nextra::cron::CronConfig loadCfg(const nlohmann::json& j)
{
    nextra::cron::CronConfig cfg;
    cfg.tickInterval = std::chrono::seconds{
        j.value("tickIntervalSeconds", 30)};
    cfg.gracefulShutdown = std::chrono::seconds{
        j.value("gracefulShutdownSeconds", 10)};
    cfg.dueSlack = std::chrono::seconds{j.value("dueSlackSeconds", 1)};
    return cfg;
}
}  // namespace

namespace commands
{

void cmdCronManager(const std::string& config)
{
    std::signal(SIGINT, onSignal);
    std::signal(SIGTERM, onSignal);

    drogon::app().loadConfigFile(config);
    std::promise<void> started;
    auto startedFuture = started.get_future();
    drogon::app().registerBeginningAdvice(
        [&started] { started.set_value(); });
    std::thread httpThread([] { drogon::app().run(); });
    startedFuture.wait();
    auto db = drogon::app().getDbClient();

    std::ifstream f("constants/cron-manager.json");
    if (!f) throw std::runtime_error("cannot open cron-manager.json");
    nlohmann::json j;
    f >> j;

    nextra::cron::CronManager manager(db, loadCfg(j));
    manager.upsertSeedSchedules(j.value("seedSchedules",
                                         nlohmann::json::array()));
    nextra::cron::g_cronManager = &manager;
    manager.start();
    spdlog::info("cron-manager daemon ready");

    while (!g_stop.load())
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

    spdlog::info("cron-manager: shutdown signal received");
    drogon::app().quit();
    if (httpThread.joinable()) httpThread.join();
    manager.stop();
    nextra::cron::g_cronManager = nullptr;
}

}  // namespace commands
