/**
 * @file session_store.cpp
 * @brief SessionStore: createSession + isSessionValid.
 *
 * Fail-closed policy: on any DB error, ok(false) is called
 * so the request is denied rather than silently allowed.
 */
#include "SessionStore.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <string>

namespace services::auth
{

void SessionStore::createSession(
    const std::string& jti,
    const std::string& userId,
    const std::string& expiresAt,
    const std::string& userAgent,
    const std::string& ip,
    SessionOkCb ok,
    SessionErrCb err)
{
    auto db = drogon::app().getDbClient();
    *db << "INSERT INTO user_sessions"
           " (jti, user_id, expires_at,"
           "  user_agent, ip_address)"
           " VALUES ($1,$2,$3::timestamptz,$4,$5)"
        << jti << userId << expiresAt
        << userAgent << ip
        >> [ok](const drogon::orm::Result&) {
            ok(true);
        }
        >> [err, jti](
               const drogon::orm::DrogonDbException& e) {
            spdlog::error(
                "session create jti={}: {}",
                jti, e.base().what());
            err(e.base().what());
        };
}

void SessionStore::isSessionValid(
    const std::string& jti,
    SessionOkCb ok,
    SessionErrCb err)
{
    // TODO(v2): cache session validity in Redis to avoid a
    // DB round-trip on every authenticated request.
    auto db = drogon::app().getDbClient();
    *db << "SELECT 1 FROM user_sessions"
           " WHERE jti=$1"
           "   AND revoked=FALSE"
           "   AND expires_at > NOW()"
           " LIMIT 1"
        << jti
        >> [ok](const drogon::orm::Result& r) {
            ok(!r.empty());
        }
        >> [ok, err, jti](
               const drogon::orm::DrogonDbException& e) {
            spdlog::error(
                "session valid check jti={}: {}",
                jti, e.base().what());
            // Fail closed: deny access on DB error.
            ok(false);
            err(e.base().what());
        };
}

} // namespace services::auth
