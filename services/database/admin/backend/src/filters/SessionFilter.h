/**
 * @file SessionFilter.h
 * @brief JWT session validation filter.
 */

#pragma once

#include <drogon/HttpFilter.h>

namespace pgadmin
{

/// @brief Validates Bearer JWT from Authorization
///        header. Sets "username" attribute.
class SessionFilter
    : public drogon::HttpFilter<SessionFilter>
{
  public:
    void doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& cb,
        drogon::FilterChainCallback&& ccb) override;
};

} // namespace pgadmin
