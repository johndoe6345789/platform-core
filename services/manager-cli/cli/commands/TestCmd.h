/**
 * @file TestCmd.h
 * @brief Test subcommand - builds and runs project tests
 *        with optional name filtering.
 */

#pragma once

#include <string>

namespace manager
{

/**
 * @class TestCmd
 * @brief Builds the test target and runs ctest.
 */
class TestCmd
{
  public:
    /**
     * @brief Build tests and run ctest.
     *
     * @param filter Optional test name filter pattern.
     *               Passed to ctest --tests-regex.
     * @return int 0 on success, non-zero on failure.
     */
    static int execute(const std::string& filter = "");
};

} // namespace manager
