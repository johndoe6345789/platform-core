#pragma once
/**
 * @file auth_client.h
 * @brief Validate a request's credential via the auth service.
 *
 * The auth domain is treated as a separate service (its code
 * is NOT linked here). This makes an internal HTTP call to the
 * auth service's `GET /api/auth/validate` endpoint, forwarding
 * the caller's Authorization header and cookies. Fails closed:
 * any transport error or non-200 denies the request.
 *
 * Target host/port come from AUTH_HOST / AUTH_PORT (default
 * businessplanner-auth:9001).
 */

#include <drogon/drogon.h>

#include <functional>
#include <string>

namespace filters
{

/**
 * @brief Validate @p req against the auth service over HTTP.
 * @param req   Incoming request (Authorization + Cookie are
 *              forwarded).
 * @param onOk  Called with (userId, role) on a 200 response.
 * @param onFail Called with a ready 401/503 response to deny.
 */
void validateViaAuthService(
    const drogon::HttpRequestPtr& req,
    std::function<void(const std::string& userId,
                       const std::string& role)> onOk,
    std::function<void(
        const drogon::HttpResponsePtr&)> onFail);

} // namespace filters
