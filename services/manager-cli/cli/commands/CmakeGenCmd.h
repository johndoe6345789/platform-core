/**
 * @file CmakeGenCmd.h
 * @brief Embedded CMake generator - reads project.json,
 *        discovers sources, renders inja templates into
 *        CMakeLists.txt without an external binary.
 */

#pragma once

#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace manager
{

/**
 * @class CmakeGenCmd
 * @brief Generates CMakeLists.txt from project.json
 *        and inja templates, fully embedded in manager.
 */
class CmakeGenCmd
{
  public:
    /**
     * @brief Run the cmake generation pipeline.
     * @param input      Path to project.json.
     * @param output_dir Output directory.
     * @param tmpl_dir   Directory with .cmake.j2 templates.
     * @return int 0 on success, non-zero on failure.
     */
    static int execute(const std::string& input, const std::string& output_dir,
                       const std::string& tmpl_dir);

    /// @brief Build inja context from project.json.
    static nlohmann::json buildContext(const nlohmann::json& project,
                                       const std::filesystem::path& base);
};

} // namespace manager
