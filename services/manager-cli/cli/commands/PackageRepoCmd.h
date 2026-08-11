/**
 * @file PackageRepoCmd.h
 * @brief Manager subcommand for the local package repository.
 */

#pragma once

#include <CLI/CLI.hpp>

namespace manager
{

/// @brief Manages the package repository backend lifecycle.
class PackageRepoCmd
{
  public:
    static constexpr const char* kContainer = "nextra-packagerepo";
    static constexpr const char* kDbContainer = "nextra-packagerepo-db";
    static constexpr const char* kNetwork = "nextra-packagerepo-net";
    static constexpr int kPort = 5050;
    static constexpr const char* kDataVol = "nextra-packagerepo-data";
    static constexpr const char* kDbVol = "nextra-packagerepo-dbdata";

    /// @brief Build the backend (Docker image).
    static int build();

    /// @brief Start the full stack (DB + backend).
    static int up();

    /// @brief Stop the full stack.
    static int down();

    /// @brief Show server status.
    static int status();

    /// @brief Pull a package from the repository.
    /// @param spec Package spec: namespace/name@version.
    /// @param outDir Output directory (default: current dir).
    static int pull(const std::string& spec,
                    const std::string& outDir);

    /// @brief Start the Postgres container for repo.
    static int startDb();

    /// @brief Ensure S3 server is up and connected.
    static int ensureS3();

    /// @brief Register all subcommands under "repo".
    static void registerAll(CLI::App& parent);
};

} // namespace manager
