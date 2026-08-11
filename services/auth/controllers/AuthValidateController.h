#pragma once
/**
 * @file AuthValidateController.h
 * @brief SSO token-validation endpoint for nginx auth_request.
 *
 * GET /api/auth/validate
 *   Reads the `nextra_token` cookie, verifies the JWT, and
 *   returns 200 OK (valid) or 401 Unauthorized (invalid/absent).
 *   nginx uses this status code to gate access to all tool routes.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class AuthValidateController
    : public drogon::HttpController<AuthValidateController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthValidateController::validate,
                  "/api/auth/validate", drogon::Get);
    METHOD_LIST_END

    /**
     * @brief Validate the SSO cookie and return 200 or 401.
     * @param req Incoming HTTP request.
     * @param cb  Response callback.
     */
    void validate(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
