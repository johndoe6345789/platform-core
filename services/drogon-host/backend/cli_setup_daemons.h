#pragma once

/**
 * @file cli_setup_daemons.h
 * @brief CLI11 sub-command registration for the
 *        background-daemon entry points.
 *
 * Kept separate from cli_setup.h so each file stays
 * under the 100-line project limit.
 */

#include "cli_daemon_opts.h"

#include <CLI/CLI.hpp>

#include <string>

/**
 * @brief Register a single daemon sub-command.
 * @param app  Root CLI11 application.
 * @param name Sub-command name shown on the CLI.
 * @param desc Help-line description.
 * @param out  Config-path string to bind @c -c to.
 * @return Pointer to the registered sub-command.
 */
inline CLI::App* addDaemonCmd(
    CLI::App& app,
    const char* name,
    const char* desc,
    std::string& out)
{
    auto* cmd = app.add_subcommand(name, desc);
    cmd->add_option(
        "-c,--config", out,
        "Path to Drogon JSON config")
        ->default_val("config/config.json");
    return cmd;
}

/**
 * @brief Register every daemon sub-command on @p app.
 * @param app  Root CLI11 application.
 * @param opts Aggregate of daemon option values.
 * @param cmds Aggregate of sub-command handles to fill.
 */
inline void addDaemonCmds(
    CLI::App& app,
    DaemonOpts& opts,
    DaemonCmds& cmds)
{
    cmds.backup = addDaemonCmd(
        app, "backup-manager",
        "Run the backup-manager daemon",
        opts.backupConfig);
    cmds.cron = addDaemonCmd(
        app, "cron-manager",
        "Run the cron schedule materialiser",
        opts.cronConfig);
    cmds.image = addDaemonCmd(
        app, "image-processor",
        "Run the image processor daemon",
        opts.imageConfig);
    cmds.job = addDaemonCmd(
        app, "job-scheduler",
        "Run the durable job scheduler daemon",
        opts.jobConfig);
    cmds.media = addDaemonCmd(
        app, "media-streaming",
        "Run the media-streaming control plane",
        opts.mediaConfig);
    cmds.notification = addDaemonCmd(
        app, "notification-router",
        "Run the notification router daemon",
        opts.notificationConfig);
    cmds.pdf = addDaemonCmd(
        app, "pdf-generator",
        "Run the PDF generator daemon",
        opts.pdfConfig);
    cmds.search = addDaemonCmd(
        app, "search-indexer",
        "Run the search-indexer daemon",
        opts.searchConfig);
    cmds.video = addDaemonCmd(
        app, "video-transcoder",
        "Run the video-transcoder daemon",
        opts.videoConfig);
    cmds.webhook = addDaemonCmd(
        app, "webhook-dispatcher",
        "Run the webhook-dispatcher daemon",
        opts.webhookConfig);
}
