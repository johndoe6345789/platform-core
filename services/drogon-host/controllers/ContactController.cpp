/**
 * @file ContactController.cpp
 * @brief Contact form endpoint implementation.
 */

#include "ContactController.h"
#include "email/backend/EmailService.h"
#include "drogon-host/backend/utils/JsonResponse.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <string>
#include <thread>

using json = nlohmann::json;
using Cb =
    std::function<void(const drogon::HttpResponsePtr&)>;

namespace controllers
{

void ContactController::submit(
    const drogon::HttpRequestPtr& req, Cb&& cb)
{
    auto body = json::parse(
        req->bodyData(),
        req->bodyData() + req->bodyLength(),
        nullptr, false);

    if (body.is_discarded()
        || !body.contains("name")
        || !body.contains("email")
        || !body.contains("message")) {
        cb(::utils::jsonError(
            drogon::k400BadRequest,
            "name, email and message required"));
        return;
    }

    auto name = body["name"].get<std::string>();
    auto email = body["email"].get<std::string>();
    auto msg = body["message"].get<std::string>();

    spdlog::info("Contact from {} <{}>", name, email);

    // Detached thread: any uncaught exception would
    // terminate the whole process. Wrap in try/catch.
    std::thread([name, email, msg]() {
        try {
            services::EmailService svc;
            auto html = fmt::format(
                "<h3>Contact from {} &lt;{}&gt;</h3>"
                "<p>{}</p>",
                name, email, msg);
            svc.sendEmail(
                "admin@nextra.local",
                "Contact: " + name, html);
        } catch (const std::exception& e) {
            spdlog::error(
                "Contact email failed: {}",
                e.what());
        } catch (...) {
            spdlog::error(
                "Contact email failed: unknown");
        }
    }).detach();

    cb(::utils::jsonOk(
        json{{"message", "Message received"}}));
}

} // namespace controllers
