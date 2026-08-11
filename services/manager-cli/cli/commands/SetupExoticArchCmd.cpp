/**
 * @file SetupExoticArchCmd.cpp
 * @brief Patches node_modules for exotic-arch Docker builds.
 *
 * On riscv64/ppc64le, Next.js has no native binding loader
 * so we stub @parcel/watcher and patch @swc/core to use the
 * WASM fallback. Webpack is used instead of Turbopack.
 */
#include "SetupExoticArchCmd.h"
#include <cstdlib>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <string>

namespace fs = std::filesystem;
namespace manager
{

/// Write @a content to @a path, creating parent dirs.
static void writeFile(const fs::path& path, const std::string& content)
{
    fs::create_directories(path.parent_path());
    std::ofstream(path) << content;
}

/// Stub @parcel/watcher with a no-op implementation.
static void stubParcelWatcher(const fs::path& nm)
{
    auto dir = nm / "@parcel" / "watcher";
    if (std::system("node -e \"require('@parcel/watcher')\" "
                    "2>/dev/null") == 0) {
        return;
    }
    writeFile(dir / "package.json", "{\"name\":\"@parcel/watcher\","
                                    "\"main\":\"index.js\"}\n");
    writeFile(dir / "index.js", "exports.subscribe=async()=>"
                                "({unsubscribe:async()=>{}});\n"
                                "exports.getEventsSince=async()=>[];\n");
    fmt::print("Stubbed @parcel/watcher\n");
}

/// Patch nested @swc/core to fall back to @swc/wasm.
static void patchSwcCore(const fs::path& nm)
{
    std::system("npm install --no-save @swc/wasm");

    for (auto& e : fs::recursive_directory_iterator(nm)) {
        if (e.path().filename() != "binding.js")
            continue;
        if (e.path().parent_path().filename() != "core")
            continue;
        auto parent = e.path().parent_path();
        if (parent.parent_path().filename() != "@swc")
            continue;

        auto cmd = "node -e \"require('" + parent.string() + "')\" 2>/dev/null";
        if (std::system(cmd.c_str()) == 0)
            continue;

        writeFile(e.path(), "module.exports=require(\"@swc/wasm\");\n");
        auto rel = fs::relative(e.path(), nm).string();
        fmt::print("Patched {}\n", rel);
    }
}

int SetupExoticArchCmd::execute(const std::string& swcDir,
                                const std::string& nmDir)
{
    (void)swcDir;
    fs::path nm(nmDir);
    stubParcelWatcher(nm);
    patchSwcCore(nm);
    return 0;
}

} // namespace manager
