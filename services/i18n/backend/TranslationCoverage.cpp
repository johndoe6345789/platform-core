/**
 * @file TranslationCoverage.cpp
 * @brief Per-locale coverage stats for translations.
 */

#include "i18n/backend/TranslationService.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace services
{

using namespace drogon;
using namespace drogon::orm;

void TranslationService::coverage(
    const std::string& ref,
    Callback onOk, ErrCallback onErr)
{
    const std::string sql = R"(
WITH ref AS (
  SELECT namespace, key
    FROM translations
   WHERE locale = $1
),
totals AS (
  SELECT COUNT(*) AS total FROM ref
),
per_locale AS (
  SELECT t.locale,
         COUNT(*) AS present
    FROM translations t
    JOIN ref r ON t.namespace = r.namespace
              AND t.key       = r.key
   GROUP BY t.locale
)
SELECT p.locale,
       p.present,
       tt.total
  FROM per_locale p
  CROSS JOIN totals tt
 ORDER BY p.locale
)";

    *db() << sql << ref
        >> [onOk, ref](const Result& r) {
            json arr = json::array();
            for (const auto& row : r) {
                auto lc = row["locale"]
                    .as<std::string>();
                auto present =
                    row["present"].as<int64_t>();
                auto total =
                    row["total"].as<int64_t>();
                arr.push_back({
                    {"locale", lc},
                    {"present", present},
                    {"total", total},
                    {"isReference", lc == ref},
                });
            }
            onOk({{"reference", ref},
                  {"locales", arr}});
        }
        >> [onErr](const DrogonDbException& e) {
            spdlog::error("coverage: {}",
                e.base().what());
            onErr(k500InternalServerError,
                  "Internal server error");
        };
}

} // namespace services
