#pragma once

/**
 * @file cli_dispatch_daemons.h
 * @brief Dispatches the background-daemon sub-commands
 *        to their cmdXxx(config) handlers.
 *
 * Split from cli_dispatch.h so each file stays under
 * the 100-line project limit.  Exceptions raised by
 * the handler are logged via spdlog and turned into
 * an EXIT_FAILURE return code.
 */

#include "cli_daemon_opts.h"
#include "cli_dispatch_daemons_table.h"

#include <spdlog/spdlog.h>

#include <cstdlib>
#include <exception>

/**
 * @brief Dispatch whichever daemon sub-command matched.
 * @param cmds Populated daemon sub-command handles.
 * @param opts Parsed daemon option values.
 * @param out  Set to the handler's exit code on match.
 * @return @c true if a daemon command ran, else @c false.
 */
inline bool dispatchDaemonCommand(
    const DaemonCmds& cmds,
    const DaemonOpts& opts,
    int& out)
{
    for (const auto& e : daemonDispatchTable()) {
        if (*(cmds.*e.handle)) {
            try {
                e.run(opts);
                out = EXIT_SUCCESS;
            } catch (const std::exception& ex) {
                spdlog::error(
                    "{}: {}", e.name, ex.what());
                out = EXIT_FAILURE;
            }
            return true;
        }
    }
    return false;
}
