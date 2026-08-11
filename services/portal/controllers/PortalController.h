#pragma once
/**
 * @file PortalController.h
 * @brief Serves the developer portal homepage
 *        and its stylesheet, both rendered from
 *        shared/constants/tool-links.json.
 *
 * Routes:
 *   GET /           -> 302 /portal/
 *   GET /portal     -> 302 /portal/
 *   GET /portal/    -> HTML
 *   GET /portal/styles.css -> CSS
 */

#include <drogon/HttpController.h>

namespace controllers
{

class PortalController
    : public drogon::HttpController<PortalController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(PortalController::root,
                  "/", drogon::Get);
    ADD_METHOD_TO(PortalController::redirect,
                  "/portal", drogon::Get);
    ADD_METHOD_TO(PortalController::home,
                  "/portal/", drogon::Get);
    ADD_METHOD_TO(PortalController::styles,
                  "/portal/styles.css", drogon::Get);
    METHOD_LIST_END

    /** @brief Redirect "/" to "/portal/". */
    void root(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Redirect "/portal" to "/portal/". */
    void redirect(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Render the portal HTML homepage. */
    void home(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Serve the portal stylesheet. */
    void styles(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
