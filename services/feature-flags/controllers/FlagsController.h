#pragma once
/**
 * @file FlagsController.h
 * @brief Feature flag HTTP endpoints (admin + public).
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @brief Combined admin + public routes for feature flags.
 *
 * Admin routes are JWT-gated and filtered to role=admin in
 * the handler. Public routes expose deterministic flag
 * evaluation for arbitrary user ids.
 */
class FlagsController
    : public drogon::HttpController<FlagsController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        FlagsController::list,
        "/api/flags", drogon::Get,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        FlagsController::patch,
        "/api/flags/{key}", drogon::Patch,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        FlagsController::audit,
        "/api/flags/{key}/audit", drogon::Get,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        FlagsController::evaluate,
        "/api/flags/evaluate", drogon::Get);
    METHOD_LIST_END

    /** @brief Admin: list every flag. */
    void list(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Admin: patch a single flag by key. */
    void patch(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& key);

    /** @brief Admin: recent audit rows for a flag. */
    void audit(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& key);

    /** @brief Public: evaluate a flag for a given user. */
    void evaluate(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
