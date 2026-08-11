/**
 * @file GenerateCmd.cpp
 * @brief Implementation of the generate subcommand.
 *        Dispatches to embedded cmake-gen or drogon_ctl.
 */

#include "GenerateCmd.h"
#include "CmakeGenCmd.h"

#include <fmt/core.h>

#include <cstdlib>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

namespace manager
{

/// Locate project.json (tools/manager/ or project root).
static std::string find_project_json()
{
    if (fs::exists("tools/manager/project.json")) {
        return "tools/manager/project.json";
    }
    return "project.json";
}

/// Generate CMakeLists.txt using the embedded cmake-gen.
static int generate_cmake()
{
    fmt::print("[generate] Generating CMakeLists.txt ...\n");

    auto input = find_project_json();
    auto tmpl_dir = "tools/manager/templates";

    if (!fs::exists(tmpl_dir)) {
        fmt::print("[generate] Templates dir missing: {}\n", tmpl_dir);
        return 1;
    }

    int rc = CmakeGenCmd::execute(input, ".", tmpl_dir);
    if (rc == 0) {
        fmt::print("[generate] CMakeLists.txt generated.\n");
    }
    return rc;
}

/// Generate Drogon ORM models via drogon_ctl.
static int generate_models()
{
    fmt::print("[generate] Generating Drogon models ...\n");

    std::string dir = "src/models";
    fs::create_directories(dir);

    auto cmd = fmt::format("drogon_ctl create model {}", dir);
    fmt::print("[generate] $ {}\n", cmd);
    int rc = std::system(cmd.c_str());
    if (rc == 0) {
        fmt::print("[generate] Models generated in {}\n", dir);
    }
    return rc;
}

int GenerateCmd::execute(const std::string& target)
{
    if (target == "cmake") {
        return generate_cmake();
    }
    if (target == "models") {
        return generate_models();
    }

    fmt::print("[generate] Unknown target: '{}'\n"
               "[generate] Valid targets: cmake, models\n",
               target);
    return 1;
}

} // namespace manager
