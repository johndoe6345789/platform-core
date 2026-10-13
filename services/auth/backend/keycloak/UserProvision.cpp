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
    // dbal's top roles all carry admin rights.
    if (has("admin") || has("god") || has("supergod")
        || has("owner"))
        return "admin";
    if (has("moderator")) return "moderator";
    return "user";
}
}  // namespace

void ensureUserRow(const KeycloakClaims& claims)
{
    if (claims.userId.empty()) return;
    {
        std::lock_guard<std::mutex> lk(g_seenMu);
        if (g_seen.count(claims.userId)) return;
        g_seen.insert(claims.userId);
    }

    const std::string username =
        !claims.preferredUsername.empty()
            ? claims.preferredUsername
            : claims.sub;
    // dbal tokens carry no email claim; its usernames are
    // often the address the user signed up with.
    const std::string email =
        !claims.email.empty() ? claims.email
        : claims.sub.find('@') != std::string::npos
            ? claims.sub
            : (username + "@unknown.local");
    const std::string display =
        !claims.name.empty() ? claims.name : username;
    const std::string role = pickRole(claims.roles);

    // Username and email are UNIQUE. A different row may already
    // hold them (the seeded admin/testuser/system rows, or a user
    // from another identity provider), so fall back to a name
    // derived from the id instead of failing the insert. The role
    // follows the identity provider on every first sight.
    auto db = drogon::app().getDbClient();
    db->execSqlAsync(
        "INSERT INTO users (id, email, username,"
        " password_hash, display_name, role,"
        " email_confirmed)"
        " SELECT $1::uuid,"
        "  CASE WHEN EXISTS (SELECT 1 FROM users"
        "   WHERE email = $2 AND id <> $1::uuid)"
        "   THEN ($1::uuid)::text || '@oidc.local' ELSE $2 END,"
        "  CASE WHEN EXISTS (SELECT 1 FROM users"
        "   WHERE username = $3 AND id <> $1::uuid)"
        "   THEN $3 || '-' || substr(($1::uuid)::text, 1, 8)"
        "   ELSE $3 END,"
        "  'oidc', $4, $5, true"
        " ON CONFLICT (id) DO UPDATE SET role = EXCLUDED.role",
        [](const drogon::orm::Result&) {},
        [s = claims.userId](
            const drogon::orm::DrogonDbException& e) {
            spdlog::warn(
                "ensureUserRow {}: {}", s,
                e.base().what());
        },
        claims.userId, email, username, display, role);
}

}  // namespace services::auth::keycloak
