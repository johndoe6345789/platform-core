/**
 * @file DocsController.cpp
 * @brief OpenAPI spec and docs redirect.
 */

#include "DocsController.h"
#include "api-documentation/backend/openapi_spec.h"

namespace controllers
{

void DocsController::redirect(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb)
{
    auto resp =
        drogon::HttpResponse::newRedirectionResponse(
            "/apidocs/");
    cb(resp);
}

void DocsController::spec(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(const drogon::HttpResponsePtr&)>&& cb)
{
    static const std::string json =
        docs::buildSpec().dump();

    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(drogon::k200OK);
    resp->setContentTypeCode(drogon::CT_APPLICATION_JSON);
    resp->setBody(json);
    cb(resp);
}

} // namespace controllers
