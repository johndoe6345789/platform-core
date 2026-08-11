/**
 * @file main.cpp
 * @brief PgAdmin server entry point.
 */

#include "services/Globals.h"
#include <drogon/drogon.h>
#include <iostream>

int main()
{
    std::string dbConn =
        "host=localhost port=5432 "
        "dbname=pgadmin "
        "user=pgadmin password=pgadmin";
    std::string jwtSecret = "dev-secret";
    int port = 5060;

    if (auto e = std::getenv("PG_CONN"))
        dbConn = e;
    if (auto e = std::getenv("JWT_SECRET"))
        jwtSecret = e;
    if (auto e = std::getenv("PG_PORT"))
        port = std::stoi(e);

    pgadmin::Globals::init(dbConn, jwtSecret);

    drogon::app()
        .setLogPath("./")
        .setLogLevel(trantor::Logger::kInfo)
        .addListener("0.0.0.0", port)
        .setThreadNum(4)
        .run();

    return 0;
}
