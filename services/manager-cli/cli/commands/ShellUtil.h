/**
 * @file ShellUtil.h
 * @brief Shared shell helpers: run, capture, Docker check.
 */

#pragma once

#include <fmt/core.h>

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace manager
{

/// Run a command, printing it with a tag prefix.
inline int shell(const std::string& tag, const std::string& cmd)
{
    fmt::print("[{}] $ {}\n", tag, cmd);
    return std::system(cmd.c_str());
}

/// Run a command and return its trimmed stdout.
inline std::string capture(const std::string& cmd)
{
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe == nullptr) {
        return "";
    }
    char buf[256];
    while (fgets(buf, sizeof(buf), pipe) != nullptr) {
        result += buf;
    }
    pclose(pipe);
    while (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    return result;
}

/// Check if the Docker daemon is reachable.
inline bool checkDaemon()
{
    if (std::system("docker info >/dev/null 2>&1") != 0) {
        fmt::print("[docker] Docker daemon is not running\n");
        return false;
    }
    return true;
}

/// Derive GHCR registry path from git remote URL.
inline std::string detectRegistry()
{
    auto url = capture("git remote get-url origin 2>/dev/null");
    std::string repo;
    auto pos = url.find("github.com");
    if (pos != std::string::npos) {
        repo = url.substr(pos + 11);
        if (!repo.empty() && (repo[0] == ':' || repo[0] == '/'))
            repo = repo.substr(1);
        if (repo.size() > 4 && repo.substr(repo.size() - 4) == ".git")
            repo = repo.substr(0, repo.size() - 4);
    }
    if (repo.empty())
        repo = "local/nextra";
    return "ghcr.io/" + repo;
}

/// Walk up from cwd to find the repo root (.git marker).
/// Returns empty path if not found.
inline std::filesystem::path repoRoot()
{
    auto dir = std::filesystem::current_path();
    while (true) {
        if (std::filesystem::exists(dir / ".git"))
            return dir;
        auto parent = dir.parent_path();
        if (parent == dir)
            break;
        dir = parent;
    }
    return {};
}

/// Map uname -m to Docker platform string.
inline std::string hostPlatform()
{
    auto arch = capture("uname -m");
    if (arch == "x86_64")
        return "linux/amd64";
    if (arch == "aarch64" || arch == "arm64")
        return "linux/arm64";
    return "linux/" + arch;
}

} // namespace manager
