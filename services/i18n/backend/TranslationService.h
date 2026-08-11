#pragma once
/**
 * @file TranslationService.h
 * @brief CRUD operations on the translations table.
 */

#include "gamification/backend/gamification_types.h"

#include <string>

namespace services
{

/**
 * @class TranslationService
 * @brief Reads and writes i18n translations stored
 *        in PostgreSQL.
 */
class TranslationService
{
  public:
    /**
     * @brief List distinct locales in the DB.
     * @param onOk JSON array of locale strings.
     * @param onErr Error callback.
     */
    static void listLocales(
        Callback onOk, ErrCallback onErr);

    /**
     * @brief Get all translations for a locale as
     *        nested JSON matching next-intl format.
     * @param locale Language code (e.g. "en").
     * @param onOk JSON payload on success.
     * @param onErr Error callback.
     */
    static void getByLocale(
        const std::string& locale,
        Callback onOk, ErrCallback onErr);

    /**
     * @brief Upsert a single translation entry.
     * @param locale Language code.
     * @param ns Namespace (e.g. "auth").
     * @param key Dot-notation key.
     * @param value Translated text.
     * @param userId Admin user performing edit.
     * @param onOk Success callback.
     * @param onErr Error callback.
     */
    static void upsert(
        const std::string& locale,
        const std::string& ns,
        const std::string& key,
        const std::string& value,
        const std::string& userId,
        Callback onOk, ErrCallback onErr);

    /**
     * @brief Delete a translation key.
     * @param locale Language code.
     * @param ns Namespace.
     * @param key Dot-notation key.
     * @param onOk Success callback.
     * @param onErr Error callback.
     */
    static void remove(
        const std::string& locale,
        const std::string& ns,
        const std::string& key,
        Callback onOk, ErrCallback onErr);

    /**
     * @brief Per-locale coverage vs reference.
     * @param ref Reference locale (e.g. "en").
     * @param onOk JSON array of coverage stats.
     * @param onErr Error callback.
     */
    static void coverage(
        const std::string& ref,
        Callback onOk, ErrCallback onErr);

  private:
    [[nodiscard]] static auto db() -> DbClientPtr;
};

} // namespace services
