/**
 * @file OAuthIdentityLogin.cpp
 * @brief Looks up oauth_identities and delegates to upsert.
 */

#include "OAuthIdentityLogin.h"
#include "OAuthUserUpsert.h"

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace services::auth::oauth
{

void loginViaOAuth(
    const std::string& provider,
    const UserProfile& profile,
    LoginCallback cb)
{
    if (profile.subject.empty()) {
        cb(false, nullptr);
        return;
    }
    auto db = drogon::app().getDbClient();
    const std::string sql =
        "SELECT u.id, u.email, u.display_name, u.role"
        " FROM oauth_identities oi"
        " JOIN users u ON u.id = oi.user_id"
        " WHERE oi.provider = $1 AND oi.subject = $2"
        " LIMIT 1";
    db->execSqlAsync(
        sql,
        [provider, profile, cb](
            const drogon::orm::Result& r) {
            if (r.empty()) {
                ensureUserAndLink(provider, profile, cb);
                return;
            }
            const auto& row = r[0];
            auto id = row["id"].as<std::string>();
            auto email =
                row["email"].as<std::string>();
            auto dn =
                row["display_name"].as<std::string>();
            auto role = row["role"].as<std::string>();
            cb(true,
               mintOAuthSession(id, email, dn, role));
        },
        [cb](const drogon::orm::DrogonDbException& e) {
            spdlog::error("oauth login lookup: {}",
                          e.base().what());
            cb(false, nullptr);
        },
        provider, profile.subject);
}

} // namespace services::auth::oauth
