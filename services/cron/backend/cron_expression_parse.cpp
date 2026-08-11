/**
 * @file cron_expression_parse.cpp
 * @brief Five-field Vixie cron parser.
 *
 * Builds a CronExpression bitmask struct so matching is O(1) per
 * minute tick.  Aliases are rewritten to their canonical spec and
 * then parsed through the same field pipeline.
 */

#include "CronExpression.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace nextra::cron
{

namespace
{

std::uint64_t parseField(const std::string& token, int lo, int hi)
{
    std::uint64_t mask = 0;
    std::stringstream lists(token);
    std::string item;
    while (std::getline(lists, item, ','))
    {
        int step = 1;
        auto slash = item.find('/');
        if (slash != std::string::npos)
        {
            step = std::stoi(item.substr(slash + 1));
            if (step <= 0) throw std::invalid_argument("bad step: " + item);
            item = item.substr(0, slash);
        }

        int a = lo, b = hi;
        if (item == "*")
        {
            // full range
        }
        else if (auto dash = item.find('-'); dash != std::string::npos)
        {
            a = std::stoi(item.substr(0, dash));
            b = std::stoi(item.substr(dash + 1));
        }
        else
        {
            a = b = std::stoi(item);
        }

        if (a < lo || b > hi || a > b)
            throw std::invalid_argument("field out of range: " + token);

        for (int v = a; v <= b; v += step)
            mask |= (std::uint64_t{1} << v);
    }
    return mask;
}

}  // namespace

CronExpression parseCron(const std::string& expr)
{
    static const std::unordered_map<std::string, std::string> aliases{
        {"@hourly",  "0 * * * *"},
        {"@daily",   "0 0 * * *"},
        {"@weekly",  "0 0 * * 0"},
        {"@monthly", "0 0 1 * *"},
        {"@yearly",  "0 0 1 1 *"},
    };

    std::string spec = expr;
    if (auto it = aliases.find(expr); it != aliases.end()) spec = it->second;

    std::array<std::string, 5> fields;
    std::stringstream ss(spec);
    for (int i = 0; i < 5; ++i)
        if (!(ss >> fields[i]))
            throw std::invalid_argument("cron needs 5 fields: " + expr);
    std::string extra;
    if (ss >> extra) throw std::invalid_argument("extra field: " + expr);

    CronExpression cx;
    cx.minute = parseField(fields[0], 0, 59);
    cx.hour   = parseField(fields[1], 0, 23);
    cx.day    = parseField(fields[2], 1, 31);
    cx.month  = parseField(fields[3], 1, 12);
    cx.dow    = parseField(fields[4], 0, 6);
    return cx;
}

}  // namespace nextra::cron
