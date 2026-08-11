#pragma once
/**
 * @file CorsFilter.h
 * @brief CORS preflight and header injection filter.
 *
 * Reads allowed origins from Drogon custom config key
 * "cors_origins" (JSON array of strings). Falls back to "*"
 * in development.
 */

#include <drogon/HttpFilter.h>

namespace filters
{

class CorsFilter : public drogon::HttpFilter<CorsFilter>
{
  public:
    CorsFilter() = default;

    /**
     * @brief Inject CORS headers; short-circuit OPTIONS.
     * @param req  Incoming HTTP request.
     * @param cb   Callback to invoke for preflight responses.
     * @param ccb  Chain callback to continue the pipeline.
     */
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& cb,
                  drogon::FilterChainCallback&& ccb) override;
};

} // namespace filters
