/**
 * @brief JWT auth filter — delegates to the auth service.
 *
 * The auth domain is a separate service: this filter does NOT
 * link auth/backend. It forwards the request's credential to
 * the auth service's `/api/auth/validate` (Keycloak verify +
 * session-revocation check happen there) and gates the chain
 * on the result. On success exposes `user_id` / `user_role`
 * in the request attributes for downstream controllers.
 */
#include "JwtAuthFilter.h"
#include "auth_client.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <drogon/drogon.h>
#include <string>
#include <utility>

namespace filters
{

void JwtAuthFilter::doFilter(
    const drogon::HttpRequestPtr& req,
    drogon::FilterCallback&& cb,
    drogon::FilterChainCallback&& ccb)
{
    if (req->getHeader("Authorization").empty()) {
        cb(::utils::jsonError(
            drogon::k401Unauthorized,
            "Missing Authorization header",
            "AUTH_006"));
        return;
    }

    validateViaAuthService(
        req,
        [req, ccb = std::move(ccb)](
            const std::string& userId,
            const std::string& role) mutable {
            req->attributes()->insert("user_id", userId);
            req->attributes()->insert("user_role", role);
            ccb();
        },
        [cb = std::move(cb)](
            const drogon::HttpResponsePtr& deny) mutable {
            cb(deny);
        });
}

}  // namespace filters
