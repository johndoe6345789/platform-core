/**
 * @file PreferencesService.cpp
 * @brief Read/write user theme, locale, and
 *        AI provider preferences.
 */

#include "user-preferences/backend/PreferencesService.h"
#include "user-preferences/backend/preferences_queries.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto PreferencesService::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

void PreferencesService::get(
    const std::string& userId,
    Callback onOk, ErrCallback onErr)
{
    *db() << std::string(kPrefsGetSql) << userId
        >> [onOk](const Result& r) {
            if (r.empty()) {
                onOk(defaultPrefs());
                return;
            }
            onOk(prefsFromRow(r[0]));
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error(
                "PreferencesService::get: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Internal server error");
        };
}

void PreferencesService::update(
    const std::string& userId,
    const std::string& themeMode,
    const std::string& locale,
    const std::string& aiProvider,
    Callback onOk, ErrCallback onErr)
{
    *db() << std::string(kPrefsUpdateSql)
          << themeMode << locale
          << aiProvider << userId
        >> [onOk](const Result& r) {
            if (r.empty()) {
                onOk(defaultPrefs());
                return;
            }
            onOk(prefsFromRow(r[0]));
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error(
                "PreferencesService::update: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Internal server error");
        };
}

} // namespace services
