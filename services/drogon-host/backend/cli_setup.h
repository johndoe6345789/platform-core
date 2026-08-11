#pragma once

/**
 * @file cli_setup.h
 * @brief CLI11 sub-command registration helpers.
 *
 * Keeps main.cpp focused on parsing and dispatch
 * by extracting option setup into free functions.
 */

#include "cli_options.h"

#include <CLI/CLI.hpp>

/**
 * @brief Register the "migrate" sub-command.
 * @param app  Root CLI11 application.
 * @param opts Output struct filled after parse.
 * @return Pointer to the sub-command.
 */
inline CLI::App* addMigrateCmd(
    CLI::App& app, MigrateOpts& opts)
{
    auto* cmd = app.add_subcommand(
        "migrate", "Run database migrations");
    cmd->add_flag(
        "--up", opts.up,
        "Apply all pending migrations");
    cmd->add_flag(
        "--down", opts.down,
        "Roll back the most recent migration");
    cmd->add_flag(
        "--status", opts.status,
        "Show current migration state");
    return cmd;
}

/**
 * @brief Register the "seed" sub-command.
 * @param app  Root CLI11 application.
 * @param opts Output struct filled after parse.
 * @return Pointer to the sub-command.
 */
inline CLI::App* addSeedCmd(
    CLI::App& app, SeedOpts& opts)
{
    auto* cmd = app.add_subcommand(
        "seed", "Load seed / fixture data");
    cmd->add_option(
        "-f,--file", opts.file,
        "Path to a specific seed JSON file");
    return cmd;
}

/**
 * @brief Register the "create-admin" sub-command.
 * @param app  Root CLI11 application.
 * @param opts Output struct filled after parse.
 * @return Pointer to the sub-command.
 */
inline CLI::App* addAdminCmd(
    CLI::App& app, AdminOpts& opts)
{
    auto* cmd = app.add_subcommand(
        "create-admin",
        "Create an administrator account");
    cmd->add_option(
        "--email", opts.email,
        "Admin e-mail address")->required();
    cmd->add_option(
        "--password", opts.password,
        "Admin password (will be hashed)")
        ->required();
    return cmd;
}

/**
 * @brief Register the "ecommerce-api" sub-command.
 * @param app  Root CLI11 application.
 * @param opts Output struct filled after parse.
 * @return Pointer to the sub-command.
 */
inline CLI::App* addEcommerceCmd(
    CLI::App& app, EcommerceOpts& opts)
{
    auto* cmd = app.add_subcommand(
        "ecommerce-api",
        "Run the ecommerce / Stripe daemon");
    cmd->add_option(
        "-c,--config", opts.config,
        "Path to Drogon JSON config")
        ->default_val("config/config.json");
    return cmd;
}
