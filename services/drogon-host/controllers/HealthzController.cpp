/**
 * @file HealthzController.cpp
 * @brief /healthz endpoint implementation.
 */

#include "HealthzController.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "infra/backend/RedisClient.h"
#include "infra/backend/KafkaProducer.h"
#include "elasticsearch/backend/ElasticClient.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

#include <atomic>
#include <memory>

namespace controllers
{

namespace
{
constexpr const char* kOk = "ok";
constexpr const char* kDown = "down";

/** @brief Probe the primary Drogon DB client. */
const char* probeDb()
{
    try {
        auto c = drogon::app().getDbClient();
        if (!c) return kDown;
        auto r = c->execSqlSync("SELECT 1");
        return r.size() > 0 ? kOk : kDown;
    } catch (const std::exception& e) {
        spdlog::warn("healthz db: {}", e.what());
        return kDown;
    }
}
} // namespace

void HealthzController::healthz(
    const drogon::HttpRequestPtr& /*req*/,
    std::function<void(
        const drogon::HttpResponsePtr&)>&& cb)
{
    nlohmann::json body;
    body["db"] = probeDb();
    body["redis"] =
        nextra::infra::RedisClient::instance()
                .isConnected()
            ? kOk
            : kDown;
    body["kafka"] =
        nextra::infra::KafkaProducer::instance()
                .isConnected()
            ? kOk
            : kDown;

    auto state = std::make_shared<nlohmann::json>(
        std::move(body));
    auto cbShared = std::make_shared<
        std::function<void(
            const drogon::HttpResponsePtr&)>>(
        std::move(cb));

    services::ElasticClient es;
    es.health(
        [state, cbShared](nlohmann::json) {
            (*state)["elasticsearch"] = kOk;
            (*cbShared)(::utils::jsonOk(*state));
        },
        [state, cbShared](int, std::string) {
            (*state)["elasticsearch"] = kDown;
            (*cbShared)(::utils::jsonOk(*state));
        });
}

} // namespace controllers
