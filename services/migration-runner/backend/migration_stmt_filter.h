#pragma once
/**
 * @file migration_stmt_filter.h
 * @brief Statement / error predicates for the migrator.
 */

#include <string>

namespace services::migrations
{

/** @brief Bare BEGIN/COMMIT/ROLLBACK/START/END statement. */
[[nodiscard]] bool isTxnCtl(const std::string& s);

/** @brief Benign idempotent-replay ("already exists") error. */
[[nodiscard]] bool isBenignDup(const std::string& msg);

} // namespace services::migrations
