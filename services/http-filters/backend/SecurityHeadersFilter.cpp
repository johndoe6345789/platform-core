/**
 * @file SecurityHeadersFilter.cpp
 * @brief SecurityHeadersFilter implementation.
 */

#include "http-filters/backend/SecurityHeadersFilter.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace nextra::filters
{

void SecurityHeadersFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& /*cb*/,
    drogon::FilterChainCallback&& ccb)
{
    spdlog::debug(
        "SecurityHeadersFilter: marking request {}",
        req->getPath());
    req->attributes()->insert(
        "security_headers", true);
    ccb();
}

} // namespace nextra::filters
