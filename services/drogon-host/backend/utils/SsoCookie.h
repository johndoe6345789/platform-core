#pragma once
/**
 * @file SsoCookie.h
 * @brief Name of the browser cookie that carries the SSO
 *        access token (SSO_COOKIE_NAME, default nextra_sso).
 */

#include <cstdlib>
#include <string>

namespace utils
{

/** @brief Cookie holding the SSO access token. */
inline std::string ssoCookieName()
{
    const char* v = std::getenv("SSO_COOKIE_NAME");
    return v && *v ? v : "nextra_sso";
}

/** @brief Cookie holding an admin's token while impersonating. */
inline std::string ssoAdminCookieName()
{
    return ssoCookieName() + "_admin";
}

} // namespace utils
