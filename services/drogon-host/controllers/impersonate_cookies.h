#pragma once
/**
 * @file impersonate_cookies.h
 * @brief Cookie builders for impersonation sessions.
 *
 * Keeps ImpersonateController.cpp under the 100-LOC
 * cap and centralises the hardened cookie flags so
 * both cookies share identical Secure/SameSite
 * posture.
 */

#include <drogon/Cookie.h>
#include <string>

namespace controllers
{

/// @brief 5-minute hard expiry for the live
///        impersonation session.
constexpr int kImpersonationTtlSec = 5 * 60;

/**
 * @brief Build the impersonation access-token
 *        cookie with hardened flags.
 * @param access  Short-lived access token value.
 * @return Configured drogon::Cookie.
 */
inline auto makeImpersonationCookie(
    const std::string& access) -> drogon::Cookie
{
    drogon::Cookie c("nextra_sso", access);
    c.setHttpOnly(true);
    c.setSecure(true);
    c.setPath("/");
    c.setSameSite(
        drogon::Cookie::SameSite::kStrict);
    c.setMaxAge(kImpersonationTtlSec);
    return c;
}

/**
 * @brief Build the backup cookie that stashes the
 *        admin's original refresh token so
 *        stopImpersonate can restore it.
 * @param adminToken  Admin's original nextra_sso.
 * @return Configured drogon::Cookie.
 */
inline auto makeAdminBackupCookie(
    const std::string& adminToken) -> drogon::Cookie
{
    drogon::Cookie c(
        "nextra_sso_admin", adminToken);
    c.setHttpOnly(true);
    c.setSecure(true);
    c.setPath("/");
    c.setSameSite(
        drogon::Cookie::SameSite::kStrict);
    c.setMaxAge(30 * 24 * 3600);
    return c;
}

} // namespace controllers
