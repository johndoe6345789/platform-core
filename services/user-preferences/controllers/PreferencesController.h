#pragma once
/**
 * @file PreferencesController.h
 * @brief REST endpoints for user display preferences
 *        (theme mode and locale).
 */

#include <drogon/HttpController.h>

namespace controllers
{

class PreferencesController
    : public drogon::HttpController<
          PreferencesController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        PreferencesController::get,
        "/api/users/me/preferences",
        drogon::Get,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        PreferencesController::update,
        "/api/users/me/preferences",
        drogon::Put,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /** @brief Return the caller's preferences. */
    void get(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Update the caller's preferences. */
    void update(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
