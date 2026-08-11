#pragma once

/**
 * @file CronTypes.h
 * @brief Shared types for the cron-manager daemon.
 *
 * The cron manager is an independent Drogon subcommand
 * (`./nextra-api cron-manager`) whose single responsibility is to
 * walk the @c scheduled_jobs table once per tick and insert due
 * rows into @c job_queue.  It does NOT run handlers itself — that
 * is the job scheduler's job.  Keeping the two concerns separate
 * lets each scale independently and makes the control plane
 * (pause/resume a schedule, back-fill a missed run) a pure
 * CRUD surface that never touches worker state.
 */

#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace nextra::cron
{

/// A row of @c scheduled_jobs loaded by the tick query.
struct ScheduledJob
{
    std::int32_t id{0};
    std::string  name;
    std::string  cron;
    std::string  handler;
    nlohmann::json payload;
    bool enabled{true};
    std::optional<std::chrono::system_clock::time_point> nextRunAt;
    std::optional<std::chrono::system_clock::time_point> lastRunAt;
    std::string description;
};

/// Outcome of a single cron tick.
struct TickStats
{
    std::size_t inspected{0};   ///< rows touched by the SELECT
    std::size_t enqueued{0};    ///< rows for which a queue row was inserted
    std::size_t skipped{0};     ///< rows skipped (disabled or bad expr)
    std::size_t errors{0};      ///< rows whose enqueue or reschedule failed
};

}  // namespace nextra::cron
