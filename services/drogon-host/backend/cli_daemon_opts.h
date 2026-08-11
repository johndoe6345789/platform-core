#pragma once

/**
 * @file cli_daemon_opts.h
 * @brief Option structs and subcommand handles for the
 *        background-daemon sub-commands wired through
 *        cli_setup_daemons.h and cli_dispatch_daemons.h.
 *
 * Every daemon shares the same CLI shape: a single
 * `-c,--config` option pointing at the Drogon JSON
 * config.  Keeping them in one aggregate struct avoids
 * a dozen near-identical declarations in main.cpp.
 */

#include <CLI/CLI.hpp>

#include <string>

/**
 * @brief Parsed options for every daemon sub-command.
 *
 * Each field is the config path for one daemon, defaulted
 * to the standard @c config/config.json location.
 */
struct DaemonOpts
{
    std::string backupConfig{"config/config.json"};
    std::string cronConfig{"config/config.json"};
    std::string imageConfig{"config/config.json"};
    std::string jobConfig{"config/config.json"};
    std::string mediaConfig{"config/config.json"};
    std::string notificationConfig{
        "config/config.json"};
    std::string pdfConfig{"config/config.json"};
    std::string searchConfig{"config/config.json"};
    std::string videoConfig{"config/config.json"};
    std::string webhookConfig{"config/config.json"};
};

/**
 * @brief CLI11 sub-command handles for each daemon.
 *
 * Filled in by @ref addDaemonCmds and later inspected
 * by @ref dispatchDaemonCommand to decide which handler
 * to invoke.
 */
struct DaemonCmds
{
    CLI::App* backup{nullptr};
    CLI::App* cron{nullptr};
    CLI::App* image{nullptr};
    CLI::App* job{nullptr};
    CLI::App* media{nullptr};
    CLI::App* notification{nullptr};
    CLI::App* pdf{nullptr};
    CLI::App* search{nullptr};
    CLI::App* video{nullptr};
    CLI::App* webhook{nullptr};
};
