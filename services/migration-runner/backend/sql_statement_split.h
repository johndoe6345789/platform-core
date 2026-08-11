#pragma once
/**
 * @file sql_statement_split.h
 * @brief Split SQL into individual statements.
 *
 * Splits on top-level `;` only, ignoring separators that fall
 * inside single-quoted strings, double-quoted identifiers,
 * `$tag$ ... $tag$` dollar-quoted blocks, `--` line comments
 * and block comments. Required because Drogon 1.9.3 sends every
 * execSqlAsync call through the extended-query protocol, which
 * rejects multi-command strings — so each migration (including
 * the bootstrap) must be applied one statement at a time.
 */

#include <string>
#include <vector>

namespace services
{

/**
 * @brief Split @p sql into individual statements.
 *
 * @param sql One or more SQL statements separated by `;`.
 * @return Trimmed, non-empty statements with no trailing `;`.
 */
[[nodiscard]] std::vector<std::string>
splitSqlStatements(const std::string& sql);

} // namespace services
