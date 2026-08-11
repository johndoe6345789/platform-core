#pragma once
/**
 * @file OAuthController.h
 * @brief Generic OAuth authorize + callback routes.
 */

#include <drogon/HttpController.h>
#include <string>

namespace controllers
{

class OAuthController
    : public drogon::HttpController<OAuthController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        OAuthController::authorize,
        "/api/auth/oauth/{provider}/authorize",
        drogon::Get);
    ADD_METHOD_TO(
        OAuthController::callback,
        "/api/auth/oauth/{provider}/callback",
        drogon::Get);
    METHOD_LIST_END

    /** @brief Start the authorize flow for a provider. */
    void authorize(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& provider);

    /** @brief Handle provider redirect with code + state. */
    void callback(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& provider);
};

} // namespace controllers
