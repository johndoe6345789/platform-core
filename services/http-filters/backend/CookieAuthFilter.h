#pragma once
/**
 * @file CookieAuthFilter.h
 * @brief Auth filter that reads the nextra_sso cookie.
 *
 * Used by endpoints accessed from the email client
 * frontend, which shares the SSO cookie but cannot
 * send a Bearer header (cookie is HttpOnly).
 *
 * Falls back to Bearer token if cookie is absent,
 * so the same endpoints work from both contexts.
 */

#include <drogon/HttpFilter.h>

namespace filters
{

class CookieAuthFilter
    : public drogon::HttpFilter<CookieAuthFilter>
{
  public:
    CookieAuthFilter() = default;

    /**
     * @brief Validate nextra_sso cookie or Bearer.
     * @param req  Incoming HTTP request.
     * @param cb   Failure callback.
     * @param ccb  Success chain callback.
     */
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& cb,
        drogon::FilterChainCallback&& ccb
    ) override;
};

} // namespace filters
