#pragma once
/**
 * @file ApiKeyController.h
 * @brief User API key management and admin system
 *        settings endpoints.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class ApiKeyController
    : public drogon::HttpController<ApiKeyController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        ApiKeyController::getAll,
        "/api/users/me/api-keys",
        drogon::Get,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        ApiKeyController::upsert,
        "/api/users/me/api-keys",
        drogon::Put,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        ApiKeyController::remove,
        "/api/users/me/api-keys/{provider}",
        drogon::Delete,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        ApiKeyController::getSystemKeys,
        "/api/admin/system-keys",
        drogon::Get,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        ApiKeyController::setSystemKey,
        "/api/admin/system-keys",
        drogon::Put,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /** @brief Get all user API keys (masked). */
    void getAll(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Set or update a user API key. */
    void upsert(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Delete a user API key. */
    void remove(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& provider);

    /** @brief Get system-wide API keys (admin). */
    void getSystemKeys(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Set a system-wide API key (admin). */
    void setSystemKey(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
