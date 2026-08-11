#pragma once
/**
 * @file JsonResponse.h
 * @brief Header-only helpers for building JSON
 *        HTTP responses.
 */

#include "json_response_paginated.h"

#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>
#include <string>

namespace utils
{

using json = nlohmann::json;
using HttpResponsePtr = drogon::HttpResponsePtr;

/**
 * @brief Build a 200 OK JSON response.
 * @param data  Payload to send.
 * @return Ready-to-send HTTP response.
 */
inline auto jsonOk(
    const json& data) -> HttpResponsePtr
{
    auto resp =
        drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(
        drogon::CT_APPLICATION_JSON);
    resp->setBody(data.dump());
    return resp;
}

/**
 * @brief Build a 201 Created JSON response.
 * @param data  Payload to send.
 * @return Ready-to-send HTTP response.
 */
inline auto jsonCreated(
    const json& data) -> HttpResponsePtr
{
    auto resp =
        drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k201Created);
    resp->setContentTypeCode(
        drogon::CT_APPLICATION_JSON);
    resp->setBody(data.dump());
    return resp;
}

/**
 * @brief Build an error JSON response.
 * @param status  HTTP status code.
 * @param message Human-readable error message.
 * @param code    Machine-readable error code.
 * @return Ready-to-send HTTP response.
 */
inline auto jsonError(
    drogon::HttpStatusCode status,
    const std::string& message,
    const std::string& code = {})
    -> HttpResponsePtr
{
    json body = {{"error", message}};
    if (!code.empty()) {
        body["code"] = code;
    }
    auto resp =
        drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(status);
    resp->setContentTypeCode(
        drogon::CT_APPLICATION_JSON);
    resp->setBody(body.dump());
    return resp;
}

} // namespace utils
