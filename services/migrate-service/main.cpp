/**
 * @file main.cpp
 * @brief nextra-migrate — one-shot DB migration entry point.
 *
 * Invokes the migrate command directly (NOT runService/HTTP):
 * applies all pending migrations then exits. Best-effort —
 * the runner logs+continues on conflicts and quits 0 on
 * completion, which is the desired one-shot sidecar behavior.
 */
#include "migration-runner/backend/commands/migrate.h"

int main(int argc, char* argv[])
{
    const char* cfg = argc > 1
        ? argv[1] : "config/config.json";
    commands::cmdMigrate(true, false, false, cfg);
    return 0;
}
