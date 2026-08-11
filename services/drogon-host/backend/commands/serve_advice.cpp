/**
 * @file serve_advice.cpp
 * @brief Request ID generation and CORS header injection.
 */

#include "serve_advice.h"
#include "auth/backend/passkeys/ChallengeStore.h"

#include <chrono>
#include <drogon/drogon.h>
#include <string>

namespace
{

/// @brief Attach a unique request ID to every request.
void registerRequestIdAdvice()
{
    drogon::app().registerPreHandlingAdvice(
        [](const drogon::HttpRequestPtr& req) {
            auto id = drogon::utils::getUuid();
            req->attributes()->insert(
                "request_id", id);
        });
}

/// @brief Inject request ID, CORS headers on responses.
void registerResponseAdvice()
{
    drogon::app().registerPreSendingAdvice(
        [](const drogon::HttpRequestPtr& req,
           const drogon::HttpResponsePtr& resp) {
            auto reqId = req->attributes()
                ->get<std::string>("request_id");
            if (!reqId.empty()) {
                resp->addHeader(
                    "X-Request-Id", reqId);
            }
            auto origin = req->attributes()
                ->get<std::string>("cors_origin");
            if (origin.empty()) {
                return;
            }
            resp->addHeader(
                "Access-Control-Allow-Origin", origin);
            resp->addHeader(
                "Access-Control-Allow-Methods",
                "GET, POST, PUT, PATCH, DELETE, OPTIONS");
            resp->addHeader(
                "Access-Control-Allow-Headers",
                "Content-Type, Authorization");
            resp->addHeader(
                "Access-Control-Allow-Credentials",
                "true");
            resp->addHeader(
                "Access-Control-Expose-Headers",
                "X-Request-Id");
        });
}

} // anonymous namespace

namespace commands
{

/// @brief Schedule a periodic sweep of expired WebAuthn
///        challenges so in-memory TTL is enforced.  The
///        registration is deferred to after drogon start
///        so the main event loop exists.
static void registerChallengeSweep()
{
    drogon::app().registerBeginningAdvice([] {
        drogon::app().getLoop()->runEvery(
            std::chrono::seconds(60), [] {
                namespace pk =
                    services::auth::passkeys;
                pk::ChallengeStore::instance()
                    .sweep(300);
            });
    });
}

void registerHttpAdvice()
{
    registerRequestIdAdvice();
    registerResponseAdvice();
    registerChallengeSweep();
}

} // namespace commands
