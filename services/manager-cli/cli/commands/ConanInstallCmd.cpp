/**
 * @file ConanInstallCmd.cpp
 * @brief Two-pass conan install for exotic architectures.
 */
#include "ConanInstallCmd.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;
namespace manager
{

/// Run a command, printing and capturing stdout+stderr.
static int runCapture(const std::string& cmd, std::string& output)
{
    FILE* pipe = popen((cmd + " 2>&1").c_str(), "r");
    if (!pipe)
        return -1;
    std::array<char, 4096> buf{};
    while (fgets(buf.data(), buf.size(), pipe)) {
        fmt::print("{}", buf.data());
        output += buf.data();
    }
    return pclose(pipe);
}

/// Extract cmake version from "cmake/X.Y.Z ... Invalid".
static std::string extractCmakeVer(const std::string& log)
{
    std::regex re(R"(cmake/([0-9][0-9.]*).*[Ii]nvalid)");
    std::smatch m;
    if (std::regex_search(log, m, re))
        return m[1].str();
    return {};
}

/// Append cmake to [platform_tool_requires] in profile.
static void patchProfile(const std::string& ver)
{
    const char* home = std::getenv("HOME");
    if (!home)
        home = "/root";
    auto path = fs::path(home) / ".conan2/profiles/default";
    std::ofstream(path, std::ios::app)
        << "\n[platform_tool_requires]\ncmake/" << ver << "\n";
    fmt::print("[conan-install] Pinned cmake/{}\n", ver);
}

int ConanInstallCmd::execute(const std::vector<std::string>& args)
{
    std::string cmd = "conan install .";
    for (const auto& a : args)
        cmd += " " + a;
    fmt::print("[conan-install] $ {}\n", cmd);

    std::string output;
    int rc = runCapture(cmd, output);
    if (rc == 0)
        return 0;

    auto ver = extractCmakeVer(output);
    if (ver.empty()) {
        fmt::print("[conan-install] Failed\n");
        return rc;
    }
    fmt::print("[conan-install] Retrying with "
               "cmake/{} as platform tool\n",
               ver);
    patchProfile(ver);
    output.clear();
    return runCapture(cmd, output);
}

} // namespace manager
