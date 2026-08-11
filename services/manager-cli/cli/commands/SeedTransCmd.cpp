/**
 * @file SeedTransCmd.cpp
 * @brief Registration of the seed-trans subcommand.
 */

#include "SeedTransCmd.h"

#include "ShellUtil.h"

namespace manager
{

void SeedTransCmd::registerAll(CLI::App& parent)
{
    auto* cmd = parent.add_subcommand(
        "seed-trans",
        "Generate translation INSERTs from JSON");

    static std::string dir;
    static std::string out;

    cmd->add_option(
        "--dir,-d", dir,
        "Messages directory (default: auto-detect)");

    cmd->add_option(
        "--output,-o", out,
        "Output file (default: stdout)");

    cmd->callback([]() {
        std::filesystem::path msgDir;
        if (dir.empty()) {
            auto root = repoRoot();
            msgDir = root / "backend" / "seed"
                     / "translations";
        } else {
            msgDir = dir;
        }
        generate(msgDir, out);
    });
}

} // namespace manager
