#pragma once
/**
 * @file SecurityHeadersFilter.h
 * @brief Drogon filter that stamps a request attribute so
 *        the pre-sending advice injects security headers.
 *
 * Security headers (CSP, X-Frame-Options, etc.) cannot be
 * added inside a filter because filters run before the
 * handler and the response object does not exist yet.
 * Instead this filter sets the "security_headers" attribute
 * on the request; registerSecurityHeadersAdvice() (in
 * security_headers_advice.cpp) reads it and appends the
 * headers to every outgoing response.
 */

#include <drogon/HttpFilter.h>

namespace nextra::filters
{

/**
 * @class SecurityHeadersFilter
 * @brief Marks requests for security-header injection.
 */
class SecurityHeadersFilter
    : public drogon::HttpFilter<SecurityHeadersFilter>
{
  public:
    SecurityHeadersFilter() = default;

    /**
     * @brief Set "security_headers" attribute and continue.
     * @param req Incoming HTTP request.
     * @param cb  Short-circuit response callback (unused).
     * @param ccb Continue-chain callback.
     */
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& cb,
        drogon::FilterChainCallback&& ccb) override;
};

} // namespace nextra::filters
