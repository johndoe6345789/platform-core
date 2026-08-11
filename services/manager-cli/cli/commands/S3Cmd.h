/**
 * @file S3Cmd.h
 * @brief Manager subcommand for the S3-compatible server.
 */

#pragma once

#include <CLI/CLI.hpp>

namespace manager
{

/// @brief Manages the S3 server lifecycle.
class S3Cmd
{
  public:
    static constexpr const char* kContainer = "nextra-s3server";
    static constexpr const char* kDbContainer = "nextra-s3server-db";
    static constexpr const char* kNetwork = "nextra-s3server-net";
    static constexpr int kPort = 9000;
    static constexpr const char* kDataVol = "nextra-s3server-data";
    static constexpr const char* kDbVol = "nextra-s3server-dbdata";

    /// @brief Build the S3 server Docker image.
    static int build();

    /// @brief Start the full stack (DB + S3 server).
    static int up();

    /// @brief Stop the full stack.
    static int down();

    /// @brief Show server status.
    static int status();

    /// @brief Register all subcommands under "s3".
    static void registerAll(CLI::App& parent);
};

} // namespace manager
