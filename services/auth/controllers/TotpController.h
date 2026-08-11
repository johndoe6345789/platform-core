#pragma once
/**
 * @file TotpController.h
 * @brief TOTP 2FA enrolment, verification and recovery routes.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class TotpController
    : public drogon::HttpController<TotpController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        TotpController::enroll,
        "/api/auth/totp/enroll",
        drogon::Post,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        TotpController::verify,
        "/api/auth/totp/verify",
        drogon::Post,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        TotpController::recovery,
        "/api/auth/totp/recovery",
        drogon::Post);
    METHOD_LIST_END

    /** @brief Generate a new secret + recovery codes. */
    void enroll(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Confirm a TOTP code and enable 2FA. */
    void verify(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Authenticate via a recovery code. */
    void recovery(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
