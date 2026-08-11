#pragma once
/**
 * @file json_response_paginated.h
 * @brief Paginated JSON response helper.
 */

#include <cstdint>
#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>

namespace utils
{

using json = nlohmann::json;
using HttpResponsePtr = drogon::HttpResponsePtr;

/**
 * @brief Build a paginated JSON response.
 * @param data    Array of items for this page.
 * @param total   Total items across all pages.
 * @param page    Current page number (1-based).
 * @param perPage Number of items per page.
 * @return Ready-to-send HTTP response.
 */
inline auto jsonPaginated(
    const json& data,
    std::int64_t total,
    std::int64_t page,
    std::int64_t perPage) -> HttpResponsePtr
{
    json body = {
        {"data", data},
        {"pagination",
         {{"total", total},
          {"page", page},
          {"per_page", perPage},
          {"total_pages",
           (total + perPage - 1) / perPage}}}};
    auto resp =
        drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(
        drogon::CT_APPLICATION_JSON);
    resp->setBody(body.dump());
    return resp;
}

} // namespace utils
