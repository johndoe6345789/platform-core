/**
 * @file Globals.h
 * @brief PgAdmin global state initialization.
 */

#pragma once

#include "DbPool.h"
#include <string>

namespace pgadmin
{

/// @brief Central service registry for pgadmin.
struct Globals {
    static inline std::string jwtSecret;

    /// @brief Initialize all services.
    /// @param dbConn PostgreSQL connection string
    /// @param secret JWT signing secret
    static void init(
        const std::string& dbConn,
        const std::string& secret)
    {
        jwtSecret = secret;
        DbPool::init(dbConn);
    }
};

} // namespace pgadmin
