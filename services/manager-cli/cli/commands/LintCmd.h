/**
 * @file LintCmd.h
 * @brief Lint and format subcommands - runs clang-format
 *        and clang-tidy on project source files.
 */

#pragma once

namespace manager
{

/**
 * @class LintCmd
 * @brief Discovers .cpp and .h files and runs static
 *        analysis / formatting tools on them.
 */
class LintCmd
{
  public:
    /**
     * @brief Run clang-format --dry-run and clang-tidy.
     *
     * Reports formatting violations and static analysis
     * warnings without modifying files.
     *
     * @return int 0 if clean, non-zero if issues found.
     */
    static int lint();

    /**
     * @brief Run clang-format -i to fix formatting.
     *
     * Modifies files in place.
     *
     * @return int 0 on success, non-zero on failure.
     */
    static int format();
};

} // namespace manager
