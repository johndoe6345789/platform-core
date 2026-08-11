/**
 * @file RateLimitBucketFilter.cpp
 * @brief Implementation of nextra::filters::RateLimitFilter.
 */

#include "http-filters/backend/RateLimitBucketFilter.h"
#include "rate-limit/backend/RedisBucketStore.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/HttpResponse.h>
#include <nlohmann/json.hpp>

namespace nextra::filters
{

using nextra::ratelimit::BucketSpec;
using nextra::ratelimit::RateLimitKey;
using nextra::ratelimit::RedisBucketStore;

std::string RateLimitFilter::classifyGroup(
    const std::string& path)
{
    if (path.rfind("/api/admin/", 0) == 0 ||
        path.rfind("/api/impersonate", 0) == 0) {
        return "admin";
    }
    if (path.rfind("/api/auth/login", 0) == 0 ||
        path.rfind("/api/auth/register", 0) == 0 ||
        path.rfind("/api/auth/password", 0) == 0 ||
        path.rfind("/api/auth/token", 0) == 0) {
        return "auth";
    }
    return "public";
}

std::string RateLimitFilter::tenantOf(
    const drogon::HttpRequestPtr& req)
{
    const auto& hdr = req->getHeader("X-Tenant-Id");
    if (!hdr.empty()) {
        return hdr;
    }
    const auto& user = req->getHeader("X-User-Id");
    if (!user.empty()) {
        return user;
    }
    return req->getPeerAddr().toIp();
}

BucketSpec RateLimitFilter::specFor(
    const std::string& group)
{
    // Limits match constants/rate-limit.json; hard-coded here
    // so the filter has no startup IO dependency.
    if (group == "admin") {
        return {60, 1.0};
    }
    if (group == "auth") {
        return {10, 0.1666};
    }
    return {600, 10.0};
}

void RateLimitFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& cb,
    drogon::FilterChainCallback&& ccb)
{
    const std::string path = req->getPath();
    const std::string group = classifyGroup(path);
    const RateLimitKey key{tenantOf(req), group};
    const BucketSpec spec = specFor(group);
    const auto result = RedisBucketStore::take(key, spec);
    if (result.allowed) {
        ccb();
        return;
    }
    nlohmann::json body = {
        {"error", "rate_limited"},
        {"group", group},
        {"retry_after", result.retryAfter}};
    auto resp = ::utils::jsonError(
        drogon::k429TooManyRequests,
        "rate limited", "rate_limited");
    resp->addHeader(
        "Retry-After",
        std::to_string(result.retryAfter));
    cb(resp);
}

} // namespace nextra::filters
