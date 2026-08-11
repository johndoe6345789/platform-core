#pragma once
/**
 * @file auth_password_helpers.h
 * @brief Callback wrappers for AuthPasswordController.
 */

#include "auth/backend/AuthService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <functional>
#include <string>

namespace controllers
{

using Cb = std::function<void(
    const drogon::HttpResponsePtr&)>;
using Ok = std::function<void(
    const services::json&)>;
using Err = services::ErrCallback;

/**
 * @brief Wrap a response callback for success.
 * @param cb  The HTTP response callback.
 * @return Service-level success callback.
 */
inline Ok okCb(Cb cb)
{
    return [cb](const services::json& r) {
        cb(::utils::jsonOk(r));
    };
}

/**
 * @brief Wrap a response callback for errors.
 * @param cb  The HTTP response callback.
 * @return Service-level error callback.
 */
inline Err errCb(Cb cb)
{
    return [cb](drogon::HttpStatusCode c,
                const std::string& m) {
        cb(::utils::jsonError(c, m));
    };
}

} // namespace controllers
