#pragma once
/**
 * @file MigrationFileUtils.h
 * @brief File-system utilities for SQL migration files.
 *
 * Provides pure file I/O helpers: directory discovery,
 * file reading, and UP/DOWN section extraction.
 * Has no database dependency.
 */

#include <string>
#include <vector>

namespace services
{

/**
 * @class MigrationFileUtils
 * @brief Static helpers for reading and parsing migration files.
 */
class MigrationFileUtils
{
  public:
    /**
     * @brief List all `.sql` files in a directory, sorted by name.
     *
     * Files are sorted lexicographically so that the numeric
     * prefix (001_, 002_, ...) determines execution order.
     *
     * @param dir Filesystem path to the migrations directory.
     * @return Sorted list of filenames (basename only).
     */
    [[nodiscard]] static auto
    discoverFiles(const std::string& dir) -> std::vector<std::string>;

    /**
     * @brief Read the entire contents of a file.
     *
     * @param path Absolute or relative path to the file.
     * @return File contents as a string.
     * @throws std::runtime_error if the file cannot be opened.
     */
    [[nodiscard]] static auto readFile(const std::string& path) -> std::string;

    /**
     * @brief Extract the UP section from migration SQL.
     *
     * Returns everything before the first `-- DOWN` marker.
     * If no marker is present the full content is returned.
     *
     * @param sql Raw file contents.
     * @return SQL to run on forward migration.
     */
    [[nodiscard]] static auto extractUp(const std::string& sql) -> std::string;

    /**
     * @brief Extract the DOWN section from migration SQL.
     *
     * Returns everything after the `-- DOWN` marker line.
     * Returns an empty string when no marker is found.
     *
     * @param sql Raw file contents.
     * @return SQL to run on rollback, or empty.
     */
    [[nodiscard]] static auto
    extractDown(const std::string& sql) -> std::string;

    /**
     * @brief Split a multi-statement SQL string into individual
     *        statements.
     *
     * Splits on `;` boundaries and trims leading whitespace from
     * each fragment. Empty fragments are discarded. No semicolons
     * are included in the returned strings.
     *
     * @param sql One or more SQL statements separated by `;`.
     * @return Non-empty individual statements, semicolons removed.
     */
    [[nodiscard]] static auto
    splitStatements(const std::string& sql)
        -> std::vector<std::string>;
};

} // namespace services
