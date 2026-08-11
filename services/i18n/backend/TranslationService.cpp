/**
 * @file TranslationService.cpp
 * @brief Read operations for translations.
 */

#include "i18n/backend/TranslationService.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

auto TranslationService::db() -> DbClientPtr
{
    return drogon::app().getDbClient();
}

/// Set a value at a dot-notation path in JSON.
static void setNested(
    json& obj, const std::string& key,
    const std::string& val)
{
    auto pos = key.find('.');
    if (pos == std::string::npos) {
        obj[key] = val;
        return;
    }
    setNested(
        obj[key.substr(0, pos)],
        key.substr(pos + 1), val);
}

void TranslationService::listLocales(
    Callback onOk, ErrCallback onErr)
{
    *db() << "SELECT DISTINCT locale "
             "FROM translations ORDER BY locale"
        >> [onOk](const Result& r) {
            json arr = json::array();
            for (const auto& row : r)
                arr.push_back(
                    row["locale"].as<std::string>());
            onOk({{"locales", arr}});
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error("listLocales: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Internal server error");
        };
}

void TranslationService::getByLocale(
    const std::string& locale,
    Callback onOk, ErrCallback onErr)
{
    const std::string sql =
        "SELECT namespace, key, value "
        "FROM translations WHERE locale = $1 "
        "ORDER BY namespace, key";

    *db() << sql << locale
        >> [onOk](const Result& r) {
            json out = json::object();
            for (const auto& row : r) {
                auto ns = row["namespace"]
                    .as<std::string>();
                auto k = row["key"]
                    .as<std::string>();
                auto v = row["value"]
                    .as<std::string>();
                setNested(out[ns], k, v);
            }
            onOk(out);
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error("getByLocale: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Internal server error");
        };
}

} // namespace services
