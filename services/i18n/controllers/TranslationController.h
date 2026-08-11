#pragma once
/**
 * @file TranslationController.h
 * @brief REST endpoints for i18n translations.
 *
 * Public GET for locale data, admin-only PUT/DELETE
 * for managing translations.
 */

#include <drogon/HttpController.h>

#include <string>

namespace controllers
{

class TranslationController
    : public drogon::HttpController<
          TranslationController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        TranslationController::listLocales,
        "/api/locales",
        drogon::Get);
    ADD_METHOD_TO(
        TranslationController::getLocale,
        "/api/translations/{locale}",
        drogon::Get);
    ADD_METHOD_TO(
        TranslationController::coverage,
        "/api/translations/coverage",
        drogon::Get,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        TranslationController::upsert,
        "/api/admin/translations",
        drogon::Put,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        TranslationController::remove,
        "/api/admin/translations/{locale}"
        "/{ns}/{key}",
        drogon::Delete,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /** @brief List available locales from DB. */
    void listLocales(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Get all translations for a locale. */
    void getLocale(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& locale);

    /** @brief Coverage stats per locale. */
    void coverage(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Upsert a translation (admin only). */
    void upsert(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Delete a translation (admin only). */
    void remove(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& locale,
        const std::string& ns,
        const std::string& key);
};

} // namespace controllers
