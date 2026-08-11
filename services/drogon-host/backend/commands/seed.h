/**
 * @file seed.h
 * @brief Declaration of the seed sub-command handler.
 * @copyright 2024 Nextra Contributors
 */

#pragma once

#include <string>

namespace commands
{

/**
 * @brief Load seed / fixture data into the database.
 *
 * When @p file is empty, all JSON files under the
 * seed/ directory are loaded.  Otherwise, only the
 * specified file is processed.
 *
 * @param file Optional path to a specific seed JSON
 *             file.  Pass an empty string to load all.
 * @throws std::runtime_error if the file does not exist.
 */
void cmdSeed(const std::string& file);

} // namespace commands
