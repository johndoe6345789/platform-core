/**
 * @file cron_manager_seeds.cpp
 * @brief CronManager::upsertSeedSchedules — load cron-manager.json seeds.
 *
 * Called by the daemon main() at boot.  Every seed row is upserted
 * on its unique name so re-running the daemon is idempotent.  The
 * enabled/cron/handler/payload columns are kept in sync with the
 * JSON file so operators can edit schedules in version control and
 * redeploy — but next_run_at and last_run_at are preserved across
 * upserts so a redeploy does NOT reset the schedule's clock.
 */

#include "CronManager.h"

#include <drogon/orm/DbClient.h>
#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

namespace nextra::cron
{

void CronManager::upsertSeedSchedules(const nlohmann::json& seeds)
{
    if (!seeds.is_array())
    {
        spdlog::warn("cron seeds: expected array, got {}", seeds.type_name());
        return;
    }

    for (const auto& s : seeds)
    {
        const std::string name = s.value("name", "");
        const std::string cron = s.value("cron", "");
        const std::string handler = s.value("handler", "");
        const std::string desc = s.value("description", "");
        const auto payload = s.value("payload", nlohmann::json::object());

        if (name.empty() || cron.empty() || handler.empty())
        {
            spdlog::warn("cron seed missing name/cron/handler: {}", s.dump());
            continue;
        }

        try
        {
            db_->execSqlSync(
                "INSERT INTO scheduled_jobs "
                "(name, cron, handler, payload, description, enabled) "
                "VALUES ($1,$2,$3,$4::jsonb,$5,TRUE) "
                "ON CONFLICT (name) DO UPDATE SET "
                "  cron = EXCLUDED.cron, "
                "  handler = EXCLUDED.handler, "
                "  payload = EXCLUDED.payload, "
                "  description = EXCLUDED.description, "
                "  updated_at = now()",
                name, cron, handler, payload.dump(), desc);
        }
        catch (const drogon::orm::DrogonDbException& e)
        {
            spdlog::error("cron seed {} upsert failed: {}",
                          name, e.base().what());
        }
    }
}

}  // namespace nextra::cron
