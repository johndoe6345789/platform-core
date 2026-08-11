#pragma once

/**
 * @file cli_options.h
 * @brief Structs for CLI11 sub-command options.
 *
 * Each struct holds the parsed values for one
 * sub-command, keeping cli_setup.h focused on
 * registration functions.
 */

#include <string>

/// @brief Options from the "migrate" sub-command.
struct MigrateOpts
{
    bool up{false};
    bool down{false};
    bool status{false};
};

/// @brief Options from the "seed" sub-command.
struct SeedOpts
{
    std::string file;
};

/// @brief Options from the "create-admin" sub-command.
struct AdminOpts
{
    std::string email;
    std::string password;
};

/// @brief Options from the "ecommerce-api" sub-command.
struct EcommerceOpts
{
    std::string config{"config/config.json"};
};
