/**
 * @file PackageRepoCmdPull.cpp
 * @brief Pull a package artifact from the local repository.
 */

#include "PackageRepoCmd.h"
#include "ShellUtil.h"

#include <fmt/core.h>

#include <filesystem>
#include <string>

namespace manager
{

/// @brief Parse "ns/name@version" into components.
/// @return true on success.
static bool parseSpec(const std::string& spec, std::string& ns,
                      std::string& name, std::string& ver)
{
    auto slash = spec.find('/');
    auto at = spec.find('@', slash == std::string::npos ? 0 : slash);
    if (slash == std::string::npos || at == std::string::npos) {
        return false;
    }
    ns = spec.substr(0, slash);
    name = spec.substr(slash + 1, at - slash - 1);
    ver = spec.substr(at + 1);
    return !ns.empty() && !name.empty() && !ver.empty();
}

int PackageRepoCmd::pull(const std::string& spec,
                         const std::string& outDir)
{
    std::string ns;
    std::string name;
    std::string ver;
    if (!parseSpec(spec, ns, name, ver)) {
        fmt::print("[repo] Invalid spec: {}\n", spec);
        fmt::print("[repo] Expected: namespace/name@version\n");
        return 1;
    }

    auto url = fmt::format(
        "http://localhost:{}/v1/{}/{}/{}/default/blob",
        kPort, ns, name, ver);

    auto dir = outDir.empty() ? "." : outDir;
    std::filesystem::create_directories(dir);
    auto outFile = fmt::format("{}/{}-{}.tar.gz", dir, name, ver);

    fmt::print("[repo] Pulling {}/{}@{} ...\n", ns, name, ver);
    fmt::print("[repo] GET {}\n", url);

    auto rc = shell("repo", fmt::format(
        "curl -fSL -o '{}' '{}'", outFile, url));
    if (rc != 0) {
        fmt::print("[repo] Pull failed (is the server running?)\n");
        fmt::print("[repo] Start with: manager repo up\n");
        return 1;
    }

    auto size = capture(fmt::format(
        "wc -c < '{}'", outFile));
    fmt::print("[repo] Saved {} ({}B)\n", outFile, size);
    return 0;
}

} // namespace manager
