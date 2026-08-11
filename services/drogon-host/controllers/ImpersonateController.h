#pragma once
/**
 * @file ImpersonateController.h
 * @brief Admin endpoint to impersonate another user.
 *
 * Issues a new nextra_sso cookie for the target user
 * so the admin can browse the app as that user.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class ImpersonateController
    : public drogon::HttpController<
          ImpersonateController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        ImpersonateController::impersonate,
        "/api/admin/auth/impersonate",
        drogon::Post,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        ImpersonateController::stopImpersonate,
        "/api/admin/auth/stop-impersonate",
        drogon::Post);
    ADD_METHOD_TO(
        ImpersonateController::status,
        "/api/auth/impersonation-status",
        drogon::Get);
    METHOD_LIST_END

    /**
     * @brief Impersonate a user by issuing their
     *        SSO cookie.
     * @param req JSON body: { userId: string }.
     * @param cb  Response callback.
     */
    void impersonate(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& cb);

    /**
     * @brief Stop impersonating: restore admin
     *        cookie from backup.
     * @param req No body required.
     * @param cb  Response callback.
     */
    void stopImpersonate(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& cb);

    /**
     * @brief Check if the current session is an
     *        impersonation (backup cookie exists).
     * @param req Incoming request.
     * @param cb  Response callback.
     */
    void status(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&
        )>&& cb);
};

} // namespace controllers
