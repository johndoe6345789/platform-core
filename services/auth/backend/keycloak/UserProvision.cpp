/**
 * @file UserProvision.cpp
 * @brief See header. Process-local cache + idempotent
 *        INSERT ON CONFLICT DO NOTHING.
 */

#include "UserProvision.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <mutex>
#include <unordered_set>

namespace services::auth::keycloak
{

namespace
{
std::mutex g_seenMu;
std::unordered_set<std::string> g_seen;

std::string pickRole(
    const std::vector<std::string>& roles)
{
    auto has = [&](const char* r) {
        for (const auto& x : roles)
            if (x == r) return true;
        return false;
    };
    if (has("admin")) return "admin";
    if (has("moderator")) return "moderator";
    return "user";
}
}  // namespace

void ensureUserRow(const KeycloakClaims& claims)
{
    if (claims.sub.empty()) return;
    {
        std::lock_guard<std::mutex> lk(g_seenMu);
        if (g_seen.count(claims.sub)) return;
        g_seen.insert(claims.sub);
    }

    const std::string username =
        !claims.preferredUsername.empty()
            ? claims.preferredUsername
            : claims.sub;
    const std::string email =
        !claims.email.empty() ? claims.email
            : (username + "@unknown.local");
    const std::string display =
        !claims.name.empty() ? claims.name : username;
    const std::string role = pickRole(claims.roles);

    auto db = drogon::app().getDbClient();
    db->execSqlAsync(
        "INSERT INTO users (id, email, username,"
        " password_hash, display_name, role,"
        " email_confirmed) VALUES"
        " ($1, $2, $3, 'keycloak', $4, $5, true)"
        " ON CONFLICT (id) DO NOTHING",
        [](const drogon::orm::Result&) {},
        [s = claims.sub](
            const drogon::orm::DrogonDbException& e) {
            spdlog::warn(
                "ensureUserRow {}: {}", s,
                e.base().what());
        },
        claims.sub, email, username, display, role);
}

}  // namespace services::auth::keycloak
