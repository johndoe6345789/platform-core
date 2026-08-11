/**
 * @file SeedTransOut.cpp
 * @brief SQL output writer for the seed-trans command.
 */

#include "SeedTransCmd.h"

#include <fmt/core.h>

#include <iostream>
#include <string>
#include <vector>

namespace manager
{

/// Escape single quotes for SQL strings.
std::string seedTransEsc(const std::string& s)
{
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c == '\'') out += "''";
        else out += c;
    }
    return out;
}

void SeedTransCmd::writeOutput(
    std::ostream& os,
    const std::vector<SeedRow>& rows)
{
    os << "-- Auto-generated translation seed.\n\n"
       << "INSERT INTO translations "
       << "(locale, namespace, key, value)\n"
       << "VALUES\n";
    for (size_t i = 0; i < rows.size(); ++i) {
        const auto& r = rows[i];
        os << fmt::format(
            "  ('{}', '{}', '{}', '{}')",
            seedTransEsc(r.locale),
            seedTransEsc(r.ns),
            seedTransEsc(r.key),
            seedTransEsc(r.val));
        os << (i + 1 < rows.size() ? ",\n" : "\n");
    }
    os << "ON CONFLICT (locale, namespace, key)\n"
       << "DO UPDATE SET value = EXCLUDED.value,\n"
       << "  updated_at = NOW();\n";
}

} // namespace manager
