/**
 * @file route_health_handler.cpp
 * @brief RouteHealthHandler — nightly route smoke test.
 *
 * HEAD-requests every route listed in the job payload.
 * Fires an alert via the alerts API for any 404 or 5xx.
 * 3xx redirects (auth-gated routes) are treated as healthy.
 */

#include "job-queue/backend/RouteHealthHandler.h"

#include <drogon/HttpClient.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <format>
#include <memory>

namespace nextra::health
{

using nextra::jobs::JobHandler;
using nextra::jobs::JobResult;
using nextra::jobs::QueuedJob;

namespace
{
void postAlert(const std::string& portalBase,
               const std::string& route, int code)
{
    nlohmann::json body{
        {"source",    "routes.health"},
        {"title",
            std::format("Route {} returned {}", route, code)},
        {"body",
            std::format("{} returned HTTP {}", route, code)},
        {"severity",  "warning"},
        {"dedupe_key",
            std::format("route:{}", route)},
    };
    auto cli = drogon::HttpClient::newHttpClient(portalBase);
    auto req = drogon::HttpRequest::newHttpRequest();
    req->setMethod(drogon::Post);
    req->setPath("/api/alerts");
    req->addHeader("Content-Type", "application/json");
    req->setBody(body.dump());
    cli->sendRequest(req, 10.0); // fire-and-forget
}
} // namespace

JobResult RouteHealthHandler::run(
    const QueuedJob& job) const
{
    const auto& p = job.payload;
    const auto portalBase = p.value(
        "portalBase", std::string("http://portal"));
    const auto locale = p.value(
        "locale", std::string("en"));
    const auto routes = p.value(
        "routes", nlohmann::json::array());

    int failures = 0;
    for (const auto& r : routes) {
        const auto route = r.get<std::string>();
        const auto path = std::format(
            "/app/{}{}", locale, route);
        auto cli = drogon::HttpClient::newHttpClient(
            portalBase);
        auto req = drogon::HttpRequest::newHttpRequest();
        req->setMethod(drogon::Head);
        req->setPath(path);
        auto [res, resp] = cli->sendRequest(req, 15.0);
        const int code =
            (res != drogon::ReqResult::Ok || !resp)
            ? 0
            : static_cast<int>(resp->statusCode());
        spdlog::info("route.health {} → {}", path, code);
        if (code == 404 || code >= 500 || code == 0) {
            ++failures;
            postAlert(portalBase, route, code);
        }
    }
    return JobResult::ok({
        {"routes_checked",
            static_cast<int>(routes.size())},
        {"failures", failures},
    });
}

JobHandler RouteHealthHandler::makeHandler()
{
    auto h = std::make_shared<RouteHealthHandler>();
    return [h](const QueuedJob& j) {
        return h->run(j);
    };
}

} // namespace nextra::health
