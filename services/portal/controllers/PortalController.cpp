/**
 * @file PortalController.cpp
 * @brief Implementation of the portal routes.
 */

#include "PortalController.h"
#include "portal/backend/portal_renderer.h"

namespace controllers
{

namespace
{

/** @brief Build a 302 redirect response. */
drogon::HttpResponsePtr redirectTo(
    const std::string& location)
{
    return drogon::HttpResponse
        ::newRedirectionResponse(location);
}

/** @brief Build a 200 text response with given body. */
drogon::HttpResponsePtr textResponse(
    const std::string& body,
    drogon::ContentType ct)
{
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(ct);
    resp->setBody(body);
    return resp;
}

} // namespace

void PortalController::root(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(redirectTo("/portal/"));
}

void PortalController::redirect(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(redirectTo("/portal/"));
}

void PortalController::home(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    static const std::string html =
        portal::renderPortalHtml();
    cb(textResponse(html, drogon::CT_TEXT_HTML));
}

void PortalController::styles(
    const drogon::HttpRequestPtr&,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    cb(textResponse(
        portal::portalStylesheet(),
        drogon::CT_TEXT_CSS));
}

} // namespace controllers
