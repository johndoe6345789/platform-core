#pragma once

/**
 * @file CronExpression.h
 * @brief Minimal five-field Vixie cron parser for the cron manager.
 *
 * Intentionally library-free so the template doesn't pull a cron
 * dependency just for the ticker.  Supported syntax:
 *   - Fields: minute(0-59) hour(0-23) day(1-31) month(1-12) dow(0-6)
 *   - Lists:  1,5,15
 *   - Ranges: 1-5
 *   - Steps:  * / 5, 1-30/2
 *   - Stars:  *
 *   - Aliases: @hourly @daily @weekly @monthly @yearly
 *
 * Unsupported (and rejected): names like MON/JAN, @reboot, seconds.
 * A malformed expression throws std::invalid_argument from parse().
 */

#include <array>
#include <chrono>
#include <cstdint>
#include <string>

namespace nextra::cron
{

/// Bitmasks for each of the five cron fields.  A set bit means the
/// field fires at that value.  64 bits is comfortably larger than
/// the widest field (days, 1-31) so we can use one type everywhere.
struct CronExpression
{
    std::uint64_t minute{0};   ///< bits 0..59
    std::uint64_t hour{0};     ///< bits 0..23
    std::uint64_t day{0};      ///< bits 1..31
    std::uint64_t month{0};    ///< bits 1..12
    std::uint64_t dow{0};      ///< bits 0..6 (0=Sunday)
};

/// Parse a five-field cron spec or a supported @alias.
/// @throws std::invalid_argument on syntax error.
CronExpression parseCron(const std::string& expr);

/// Compute the next fire time strictly after @p from.
/// Scans forward minute-by-minute; the caller should not need
/// sub-minute precision because the tick interval is >= 1 s.
std::chrono::system_clock::time_point
nextFireTime(const CronExpression& cx,
             std::chrono::system_clock::time_point from);

}  // namespace nextra::cron
