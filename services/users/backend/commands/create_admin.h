/**
 * @file create_admin.h
 * @brief Declaration of the create-admin sub-command.
 * @copyright 2024 Nextra Contributors
 */

#pragma once

#include <string>

namespace commands
{

/**
 * @brief Create an administrator user account.
 *
 * Hashes the password and inserts a new user row with
 * the admin role into the database.
 *
 * @param email    Admin e-mail address.
 * @param password Plain-text password (will be hashed).
 */
void cmdCreateAdmin(const std::string& email, const std::string& password);

} // namespace commands
