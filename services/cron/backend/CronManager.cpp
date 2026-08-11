/**
 * @file CronManager.cpp
 * @brief CronManager lifecycle — start/stop + ticker loop.
 *
 * The tick body (DB scan, enqueue, reschedule) lives in
 * cron_manager_tick.cpp so both files stay under the 100-line cap.
 */

#include "CronManager.h"

#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace nextra::cron
{

CronManager::CronManager(std::shared_ptr<drogon::orm::DbClient> db,
                         CronConfig cfg)
    : db_(std::move(db))
    , cfg_(std::move(cfg))
{
}

CronManager::~CronManager()
{
    stop();
}

void CronManager::start()
{
    if (running_.exchange(true)) return;
    tickerThread_ = std::thread([this] { tickerLoop(); });
    spdlog::info("CronManager started (tick every {}s)",
                 cfg_.tickInterval.count());
}

void CronManager::stop()
{
    if (!running_.exchange(false)) return;
    if (tickerThread_.joinable()) tickerThread_.join();
    spdlog::info("CronManager stopped");
}

void CronManager::tickerLoop()
{
    while (running_.load())
    {
        try
        {
            auto stats = runTick();
            if (stats.enqueued || stats.errors)
            {
                spdlog::info("cron tick: enqueued={} skipped={} errors={}",
                             stats.enqueued, stats.skipped, stats.errors);
            }
        }
        catch (const std::exception& e)
        {
            spdlog::error("CronManager tick error: {}", e.what());
        }

        for (int i = 0; i < cfg_.tickInterval.count() && running_.load(); ++i)
            std::this_thread::sleep_for(std::chrono::seconds{1});
    }
}

}  // namespace nextra::cron
