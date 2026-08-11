#pragma once
/**
 * @file PasskeyController.h
 * @brief WebAuthn registration and assertion endpoints.
 */

#include <drogon/HttpController.h>

namespace controllers
{

class PasskeyController
    : public drogon::HttpController<PasskeyController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        PasskeyController::registerBegin,
        "/api/auth/passkeys/register/begin",
        drogon::Post,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        PasskeyController::registerFinish,
        "/api/auth/passkeys/register/finish",
        drogon::Post,
        "filters::JwtAuthFilter");
    ADD_METHOD_TO(
        PasskeyController::assertBegin,
        "/api/auth/passkeys/assert/begin",
        drogon::Post);
    ADD_METHOD_TO(
        PasskeyController::assertFinish,
        "/api/auth/passkeys/assert/finish",
        drogon::Post);
    METHOD_LIST_END

    /** @brief Start a passkey registration ceremony. */
    void registerBegin(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Finish and persist the new credential. */
    void registerFinish(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Start an assertion (login) ceremony. */
    void assertBegin(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Finish an assertion and issue tokens. */
    void assertFinish(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
