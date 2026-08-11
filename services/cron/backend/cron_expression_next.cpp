/**
 * @file cron_expression_next.cpp
 * @brief nextFireTime — forward minute-scan over a CronExpression.
 *
 * Cron ticks at most once per minute, so an O(minutes) scan from
 * @c from is accurate and simple.  We bound the scan at ~400 days
 * (enough to cover any legal five-field spec, including the rare
 * "Feb 29 in a leap year" case) and throw if no match is found so
 * a malformed spec can't wedge the daemon in an infinite loop.
 */

#include "CronExpression.h"

#include <ctime>
#include <stdexcept>

namespace nextra::cron
{

namespace
{
constexpr int kMaxMinutesScan = 400 * 24 * 60;  // ~400 days
}  // namespace

std::chrono::system_clock::time_point
nextFireTime(const CronExpression& cx,
             std::chrono::system_clock::time_point from)
{
    using namespace std::chrono;

    // Round up to the next whole minute — cron fires at HH:MM:00.
    auto t = time_point_cast<minutes>(from) + minutes{1};

    for (int step = 0; step < kMaxMinutesScan; ++step, t += minutes{1})
    {
        const std::time_t tt = system_clock::to_time_t(t);
        std::tm bt{};
#ifdef _WIN32
        gmtime_s(&bt, &tt);
#else
        gmtime_r(&tt, &bt);
#endif

        const auto mm  = std::uint64_t{1} << bt.tm_min;
        const auto hh  = std::uint64_t{1} << bt.tm_hour;
        const auto dd  = std::uint64_t{1} << bt.tm_mday;
        const auto mo  = std::uint64_t{1} << (bt.tm_mon + 1);
        const auto dw  = std::uint64_t{1} << bt.tm_wday;

        if ((cx.minute & mm) && (cx.hour & hh) && (cx.month & mo) &&
            (cx.day & dd) && (cx.dow & dw))
        {
            return t;
        }
    }

    throw std::invalid_argument(
        "cron expression never fires within 400-day horizon");
}

}  // namespace nextra::cron
