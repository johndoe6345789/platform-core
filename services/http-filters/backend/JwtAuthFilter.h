#pragma once
/**
 * @file JwtAuthFilter.h
 * @brief Drogon filter that validates JWT Bearer tokens.
 *
 * On success, sets the "user_id" and "user_role" attributes
 * on the request object for downstream controllers.
 */

#include <drogon/HttpFilter.h>

namespace filters
{

class JwtAuthFilter : public drogon::HttpFilter<JwtAuthFilter>
{
  public:
    JwtAuthFilter() = default;

    /**
     * @brief Validate the Authorization header's Bearer token.
     * @param req  Incoming HTTP request.
     * @param cb   Callback to invoke on auth failure.
     * @param ccb  Chain callback to invoke on auth success.
     */
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& cb,
                  drogon::FilterChainCallback&& ccb) override;
};

} // namespace filters
