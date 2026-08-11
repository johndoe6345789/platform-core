/**
 * @file UserCmdReg.cpp
 * @brief Registration of `user` subcommands.
 */

#include "UserCmd.h"
#include "ShellUtil.h"

namespace manager
{

void UserCmd::registerAll(CLI::App& parent)
{
    auto* usr = parent.add_subcommand(
        "user", "User management operations");
    usr->require_subcommand(1);

    // ---- user seed ----
    auto* sd = usr->add_subcommand(
        "seed",
        "Hash passwords and emit SQL for users table");

    static std::string seedFile;
    static std::string seedOut;

    sd->add_option(
        "--file,-f", seedFile,
        "Path to users JSON (default: auto-detect)");
    sd->add_option(
        "--output,-o", seedOut,
        "Output SQL file (default: stdout)");

    sd->callback([]() {
        std::filesystem::path f;
        if (seedFile.empty()) {
            f = repoRoot() / "backend"
                / "seeds" / "users.json";
        } else {
            f = seedFile;
        }
        seed(f, seedOut);
    });

    // ---- user reset ----
    auto* rs = usr->add_subcommand(
        "reset",
        "Reset a user's password (outputs SQL UPDATE)");

    static std::string identifier;
    static std::string newPassword;
    static std::string resetOut;

    rs->add_option(
        "--user,-u", identifier,
        "Username or email address")
        ->required();
    rs->add_option(
        "--password,-p", newPassword,
        "New plaintext password")
        ->required();
    rs->add_option(
        "--output,-o", resetOut,
        "Output SQL file (default: stdout)");

    rs->callback([]() {
        reset(identifier, newPassword, resetOut);
    });
}

} // namespace manager
