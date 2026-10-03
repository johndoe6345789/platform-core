/**
 * @file session_store_revoke.cpp
 * @brief SessionStore: revokeSession + revokeAllUserSessions.
 *
 * Both mutations set revoked=TRUE and record revoked_at so
 * audit queries can reconstruct the revocation timeline.
 */
#include "SessionStore.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>
#include <string>

namespace services::auth
{

void SessionStore::revokeSession(
    const std::string& jti,
    SessionOkCb ok,
    SessionErrCb err)
{
    auto db = drogon::app().getDbClient();
    *db << "UPDATE user_sessions"
           " SET revoked=TRUE, revoked_at=NOW()"
           " WHERE jti=$1"
        << jti
        >> [ok](const drogon::orm::Result&) {
            ok(true);
        }
        >> [err, jti](
               const drogon::orm::DrogonDbException& e) {
            spdlog::error(
                "session revoke jti={}: {}",
                jti, e.base().what());
            err(e.base().what());
        };
}

void SessionStore::revokeAllUserSessions(
    const std::string& userId,
    SessionOkCb ok,
    SessionErrCb err)
{
    auto db = drogon::app().getDbClient();
    *db << "UPDATE user_sessions"
           " SET revoked=TRUE, revoked_at=NOW()"
           " WHERE user_id=$1::uuid"
           "   AND revoked=FALSE"
        << userId
        >> [ok](const drogon::orm::Result&) {
            ok(true);
        }
        >> [err, userId](
               const drogon::orm::DrogonDbException& e) {
            spdlog::error(
                "session revoke-all user={}: {}",
                userId, e.base().what());
            err(e.base().what());
        };
}

} // namespace services::auth
