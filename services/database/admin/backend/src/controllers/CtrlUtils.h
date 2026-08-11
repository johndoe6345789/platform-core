/**
 * @file CtrlUtils.h
 * @brief Shared response helpers for controllers.
 */

#pragma once

#include <drogon/HttpResponse.h>
#include <json/json.h>
#include <string>

namespace pgadmin
{

/// @brief Send a JSON error with custom status code.
/// @param cb Drogon response callback
/// @param msg Error message
/// @param code HTTP status code
inline void sendError(
    auto&& cb, const std::string& msg,
    drogon::HttpStatusCode code =
        drogon::k400BadRequest)
{
    Json::Value e;
    e["error"] = msg;
    auto r =
        drogon::HttpResponse::newHttpJsonResponse(e);
    r->setStatusCode(code);
    cb(r);
}

/// @brief Send a 500 error from an exception.
/// @param cb Drogon response callback
/// @param e The caught exception
inline void sendServerError(
    auto&& cb, const std::exception& e)
{
    sendError(cb, e.what(),
              drogon::k500InternalServerError);
}

/// @brief Send a JSON value as 200 OK.
/// @param cb Drogon response callback
/// @param v JSON value to send
inline void sendJson(
    auto&& cb, const Json::Value& v)
{
    cb(drogon::HttpResponse
           ::newHttpJsonResponse(v));
}

} // namespace pgadmin
