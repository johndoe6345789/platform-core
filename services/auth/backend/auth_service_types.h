#pragma once
/**
 * @file auth_service_types.h
 * @brief Shared type aliases for authentication services.
 */

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>

#include <functional>
#include <string>

namespace services
{

using json = nlohmann::json;
using DbClientPtr = drogon::orm::DbClientPtr;

/// Callback receiving a JSON result on success.
using Callback = std::function<void(json /*result*/)>;

/// Callback receiving an HTTP status code and message on error.
using ErrCallback =
    std::function<void(drogon::HttpStatusCode, std::string)>;

} // namespace services
