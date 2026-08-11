/**
 * @file OAuthUserUpsert.cpp
 * @brief Create-or-find user and link oauth_identities.
 */

#include "OAuthUserUpsert.h"

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <spdlog/spdlog.h>

namespace services::auth::oauth
{

namespace
{

void linkIdentity(
    const std::string& userId,
    const std::string& provider,
    const UserProfile& profile,
    LoginCallback cb,
    const std::string& role)
{
    auto db = drogon::app().getDbClient();
    const std::string sql =
        "INSERT INTO oauth_identities"
        " (user_id, provider, subject, email, avatar_url)"
        " VALUES ($1,$2,$3,$4,$5)"
        " ON CONFLICT (provider, subject) DO UPDATE"
        " SET email = EXCLUDED.email,"
        "     avatar_url = EXCLUDED.avatar_url";
    db->execSqlAsync(
        sql,
        [userId, profile, cb, role](
            const drogon::orm::Result&) {
            cb(true, mintOAuthSession(
                        userId, profile.email,
                        profile.displayName, role));
        },
        [cb](const drogon::orm::DrogonDbException& e) {
            spdlog::error("oauth link: {}",
                          e.base().what());
            cb(false, nullptr);
        },
        userId, provider, profile.subject,
        profile.email, profile.avatarUrl);
}

} // namespace

void ensureUserAndLink(
    const std::string& provider,
    const UserProfile& profile,
    LoginCallback cb)
{
    if (profile.email.empty()) {
        cb(false, nullptr);
        return;
    }
    auto db = drogon::app().getDbClient();
    const std::string sql =
        "INSERT INTO users (email, username,"
        " password_hash, display_name, email_confirmed,"
        " role, created_at, updated_at)"
        " VALUES ($1,$2,'!',$3,TRUE,'user',NOW(),NOW())"
        " ON CONFLICT (email) DO UPDATE"
        " SET updated_at = NOW()"
        " RETURNING id, role";
    auto username = profile.email;
    auto dn = profile.displayName.empty()
                  ? profile.email : profile.displayName;
    db->execSqlAsync(
        sql,
        [provider, profile, cb](
            const drogon::orm::Result& r) {
            auto id = r[0]["id"].as<std::string>();
            auto role = r[0]["role"].as<std::string>();
            linkIdentity(
                id, provider, profile, cb, role);
        },
        [cb](const drogon::orm::DrogonDbException& e) {
            spdlog::error("oauth user upsert: {}",
                          e.base().what());
            cb(false, nullptr);
        },
        profile.email, username, dn);
}

} // namespace services::auth::oauth
