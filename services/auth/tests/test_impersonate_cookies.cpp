/**
 * @file test_impersonate_cookies.cpp
 * @brief Regression test: impersonation cookies
 *        must carry hardened flags and a short
 *        expiry that cannot be refreshed.
 *
 * Guards against re-introducing the bug where the
 * impersonation session was long-lived and the
 * admin's refresh cookie was left usable.
 *
 * We test by serialising the Drogon cookie; that
 * output format is stable (RFC 6265) so we avoid
 * coupling the test to specific getter names.
 */

#include "drogon-host/controllers/impersonate_cookies.h"

#include <gtest/gtest.h>
#include <string>

using controllers::kImpersonationTtlSec;
using controllers::makeAdminBackupCookie;
using controllers::makeImpersonationCookie;

namespace
{
bool contains(const std::string& h,
              const std::string& n)
{
    return h.find(n) != std::string::npos;
}
} // namespace

TEST(ImpersonationCookies, TtlIsFiveMinutes)
{
    EXPECT_EQ(kImpersonationTtlSec, 300);
}

TEST(ImpersonationCookies, SsoCookieIsHardened)
{
    auto c = makeImpersonationCookie("abc.def.ghi");
    const auto s = c.cookieString();
    EXPECT_TRUE(contains(s, "nextra_sso=abc.def"));
    EXPECT_TRUE(contains(s, "HttpOnly"));
    EXPECT_TRUE(contains(s, "Secure"));
    EXPECT_TRUE(contains(s, "SameSite=Strict"));
    EXPECT_TRUE(contains(s, "Max-Age=300"));
}

TEST(ImpersonationCookies, BackupIsHardened)
{
    auto c = makeAdminBackupCookie("admin.rt");
    const auto s = c.cookieString();
    EXPECT_TRUE(
        contains(s, "nextra_sso_admin=admin.rt"));
    EXPECT_TRUE(contains(s, "HttpOnly"));
    EXPECT_TRUE(contains(s, "Secure"));
    EXPECT_TRUE(contains(s, "SameSite=Strict"));
}
