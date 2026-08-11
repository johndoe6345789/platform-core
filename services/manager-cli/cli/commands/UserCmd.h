/**
 * @file UserCmd.h
 * @brief User management subcommands for the manager CLI.
 *
 * Provides two subcommands under `user`:
 *   seed    — Read seeds/users.json, hash passwords,
 *             emit SQL INSERTs for the users table.
 *   reset   — Emit a single UPDATE to reset one user's
 *             password by username or email.
 */

#pragma once

#include <CLI/CLI.hpp>

#include <filesystem>
#include <string>

namespace manager
{

/**
 * @class UserCmd
 * @brief Registers user management subcommands.
 */
class UserCmd
{
  public:
    /**
     * @brief Register `user seed` and `user reset`
     *        under the parent CLI::App.
     * @param parent The root CLI::App.
     */
    static void registerAll(CLI::App& parent);

    /**
     * @brief Generate SQL INSERTs from a users JSON
     *        file, hashing each plaintext password.
     * @param seedFile Path to users JSON.
     * @param outFile  Output file (empty = stdout).
     * @return int 0 on success.
     */
    static int seed(
        const std::filesystem::path& seedFile,
        const std::string& outFile);

    /**
     * @brief Generate a password-reset UPDATE statement.
     * @param identifier Username or email address.
     * @param password   New plaintext password.
     * @param outFile    Output file (empty = stdout).
     * @return int 0 on success.
     */
    static int reset(
        const std::string& identifier,
        const std::string& password,
        const std::string& outFile);
};

} // namespace manager
