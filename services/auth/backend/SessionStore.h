#pragma once
/**
 * @file SessionStore.h
 * @brief DB-backed session registry for JWT revocation.
 *
 * All callbacks run on Drogon's I/O threads; callers must
 * not block. On DB error every query fails closed (denies
 * access) to prevent token reuse when the DB is unavailable.
 */

#include <functional>
#include <string>

namespace services::auth
{

/**
 * @brief Async callbacks used by SessionStore methods.
 */
using SessionOkCb  = std::function<void(bool)>;
using SessionErrCb = std::function<void(
    const std::string&)>;

/**
 * @class SessionStore
 * @brief Manages DB-backed session lifecycle for JWTs.
 */
class SessionStore
{
  public:
    /**
     * @brief Insert a new session row on token issuance.
     * @param jti       JWT ID claim (PRIMARY KEY).
     * @param userId    Owning user UUID.
     * @param expiresAt ISO-8601 expiry string.
     * @param userAgent Request User-Agent header value.
     * @param ip        Client IP address string.
     * @param ok        Called with true on success.
     * @param err       Called with message on failure.
     */
    static void createSession(
        const std::string& jti,
        const std::string& userId,
        const std::string& expiresAt,
        const std::string& userAgent,
        const std::string& ip,
        SessionOkCb ok,
        SessionErrCb err);

    /**
     * @brief Check whether a session is active and valid.
     *
     * Returns false if the row is missing, revoked, or past
     * expires_at. Fails closed on DB error (returns false).
     *
     * @param jti JWT ID to look up.
     * @param ok  Called with true if valid, false otherwise.
     * @param err Called with message on DB error.
     */
    static void isSessionValid(
        const std::string& jti,
        SessionOkCb ok,
        SessionErrCb err);

    /**
     * @brief Mark a single session as revoked (logout).
     * @param jti JWT ID to revoke.
     * @param ok  Called with true on success.
     * @param err Called with message on failure.
     */
    static void revokeSession(
        const std::string& jti,
        SessionOkCb ok,
        SessionErrCb err);

    /**
     * @brief Revoke all active sessions for a user.
     *
     * Used for logout-everywhere / account-compromise flows.
     *
     * @param userId User UUID whose sessions are revoked.
     * @param ok     Called with true on success.
     * @param err    Called with message on failure.
     */
    static void revokeAllUserSessions(
        const std::string& userId,
        SessionOkCb ok,
        SessionErrCb err);
};

} // namespace services::auth
