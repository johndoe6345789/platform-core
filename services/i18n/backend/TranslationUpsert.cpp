/**
 * @file TranslationUpsert.cpp
 * @brief Write operations for translations.
 */

#include "i18n/backend/TranslationService.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void TranslationService::upsert(
    const std::string& locale,
    const std::string& ns,
    const std::string& key,
    const std::string& value,
    const std::string& userId,
    Callback onOk, ErrCallback onErr)
{
    const std::string sql =
        "INSERT INTO translations "
        "(locale, namespace, key, value, updated_by) "
        "VALUES ($1, $2, $3, $4, $5::uuid) "
        "ON CONFLICT (locale, namespace, key) "
        "DO UPDATE SET value = $4, "
        "updated_by = $5::uuid, "
        "updated_at = NOW() "
        "RETURNING id";

    *db() << sql
        << locale << ns << key << value << userId
        >> [onOk](const Result& r) {
            json out = {
                {"ok", true},
                {"id", r[0]["id"].as<std::string>()}
            };
            onOk(out);
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error("translation upsert: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Internal server error");
        };
}

void TranslationService::remove(
    const std::string& locale,
    const std::string& ns,
    const std::string& key,
    Callback onOk, ErrCallback onErr)
{
    const std::string sql =
        "DELETE FROM translations "
        "WHERE locale = $1 AND namespace = $2 "
        "AND key = $3";

    *db() << sql << locale << ns << key
        >> [onOk](const Result&) {
            onOk({{"ok", true}});
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error("translation delete: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Internal server error");
        };
}

} // namespace services
