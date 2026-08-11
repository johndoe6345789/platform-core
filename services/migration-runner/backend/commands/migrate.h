/**
 * @file migrate.h
 * @brief Declaration of the migrate sub-command handler.
 * @copyright 2024 Nextra Contributors
 */

#pragma once

#include <string>

namespace commands
{

/**
 * @brief Run database migrations.
 *
 * Exactly one of the three flags should be true.
 * If none is set, a usage hint is printed.
 *
 * @param up     Apply all pending migrations.
 * @param down   Roll back the most recent migration.
 * @param status Print current migration state.
 * @param config Path to the Drogon JSON config file.
 */
void cmdMigrate(
    bool up, bool down, bool status,
    const std::string& config =
        "config/config.json");

} // namespace commands
