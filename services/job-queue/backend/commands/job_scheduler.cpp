/**
 * @file job_scheduler.cpp
 * @brief CLI subcommand implementation for the job-scheduler daemon.
 */

#include "job-queue/backend/commands/job_scheduler.h"

#include "job-queue/backend/JobScheduler.h"
#include "blog/backend/ScheduledPublisher.h"
#include "job-queue/backend/RouteHealthHandler.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <atomic>
#include <chrono>
#include <csignal>
#include <fstream>
#include <future>
#include <thread>

namespace
{
std::atomic<bool> g_stop{false};

void onSignal(int) { g_stop.store(true); }

nextra::jobs::SchedulerConfig loadConfig(const std::string& path)
{
    using namespace nextra::jobs;
    std::ifstream f(path);
    if (!f) throw std::runtime_error("cannot open " + path);
    nlohmann::json j;
    f >> j;

    SchedulerConfig cfg;
    cfg.workers         = j.value("workers", 4);
    cfg.pollInterval    = std::chrono::milliseconds{
        j.value("pollIntervalMs", 1000)};
    cfg.lockTimeout     = std::chrono::seconds{
        j.value("lockTimeoutSeconds", 300)};
    cfg.recoveryTick    = std::chrono::seconds{
        j.value("recoveryTickSeconds", 60)};
    cfg.gracefulShutdown = std::chrono::seconds{
        j.value("gracefulShutdownSeconds", 30)};
    cfg.backoff.baseDelay = std::chrono::milliseconds{
        j.value("backoffBaseMs", 1000)};
    cfg.backoff.maxDelay  = std::chrono::milliseconds{
        j.value("backoffMaxMs", 3600000)};
    cfg.workerIdPrefix = j.value("workerIdPrefix",
                                  std::string{"nextra-worker"});
    return cfg;
}
}  // namespace

namespace commands
{

void cmdJobScheduler(const std::string& config)
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

    auto cfg = loadConfig("constants/job-scheduler.json");
    nextra::jobs::JobScheduler scheduler(db, cfg);
    scheduler.registerHandler(
        "blog.publish_due",
        nextra::blog::ScheduledPublisher::makeHandler(db));
    scheduler.registerHandler(
        "routes.health_check",
        nextra::health::RouteHealthHandler::makeHandler());
    scheduler.start();
    spdlog::info("job-scheduler daemon ready ({} workers)", cfg.workers);

    while (!g_stop.load())
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

    spdlog::info("job-scheduler: shutdown signal received");
    drogon::app().quit();
    if (httpThread.joinable()) httpThread.join();
    scheduler.stop();
}

}  // namespace commands
