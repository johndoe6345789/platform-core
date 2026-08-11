#pragma once
/**
 * @file FeatureToggleController.h
 * @brief Feature toggle CRUD endpoints.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class FeatureToggleController
    : public drogon::HttpController<
          FeatureToggleController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        FeatureToggleController::list,
        "/api/features", drogon::Get);
    ADD_METHOD_TO(
        FeatureToggleController::toggle,
        "/api/features/{key}/toggle",
        drogon::Patch,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /**
     * @brief List all feature toggles.
     * @param req  HTTP request.
     * @param cb   Response callback.
     */
    void list(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /**
     * @brief Toggle a feature on or off (admin).
     * @param req  HTTP request.
     * @param cb   Response callback.
     * @param key  Feature toggle key.
     */
    void toggle(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& key);
};

} // namespace controllers
