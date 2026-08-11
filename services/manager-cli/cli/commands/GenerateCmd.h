/**
 * @file GenerateCmd.h
 * @brief Generate subcommand - invokes cmake-gen for
 *        CMake generation and drogon_ctl for model
 *        generation.
 */

#pragma once

#include <string>

namespace manager
{

/**
 * @class GenerateCmd
 * @brief Dispatches code generation tasks.
 */
class GenerateCmd
{
  public:
    /**
     * @brief Execute a generation target.
     *
     * @param target One of: "cmake", "models".
     * @return int 0 on success, non-zero on failure.
     */
    static int execute(const std::string& target);
};

} // namespace manager
