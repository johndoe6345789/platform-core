/**
 * @file AuditConsumer.cpp
 * @brief Consumer factory + decode/write loop. The adapter
 *        that bridges the audit-local @ref IKafkaConsumer
 *        shape onto @ref nextra::infra::IKafkaConsumer lives
 *        in audit_consumer_adapter.{h,cpp}.
 */

#include "audit/backend/AuditConsumer.h"
#include "audit/backend/AuditWriter.h"
#include "audit/backend/audit_consumer_adapter.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <atomic>
#include <chrono>
#include <thread>

namespace nextra::audit
{
namespace
{

AuditEvent decode(const nlohmann::json& j)
{
    AuditEvent ev;
    ev.tenantId  = j.at("tenant_id").get<std::string>();
    ev.action    = j.at("action").get<std::string>();
    ev.createdAt = j.value("created_at", std::string{});
    auto s = [&](const char* k, std::optional<std::string>& dst) {
        if (j.contains(k) && j[k].is_string())
            dst = j[k].get<std::string>();
    };
    s("actor_id",    ev.actorId);
    s("target_type", ev.targetType);
    s("target_id",   ev.targetId);
    if (j.contains("payload")) ev.payload = j["payload"];
    return ev;
}

}  // namespace

std::unique_ptr<IKafkaConsumer>
makeKafkaConsumer(const std::string& brokers,
                  const std::string& group)
{
    return std::make_unique<InfraConsumerAdapter>(
        brokers, group);
}

void runConsumerLoop(IKafkaConsumer& consumer,
                     AuditWriter& writer,
                     const std::string& topic,
                     std::atomic<bool>& stop)
{
    consumer.subscribe(topic);
    auto handle = [&writer](const std::string& raw) {
        try {
            writer.append(decode(nlohmann::json::parse(raw)));
        } catch (const std::exception& ex) {
            spdlog::warn("audit handle failed: {}", ex.what());
        }
        return true;  // always advance offset
    };
    while (!stop.load()) consumer.poll(handle, 200);
    consumer.close();
}

}  // namespace nextra::audit
