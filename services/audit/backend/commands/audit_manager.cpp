/**
 * @file audit_manager.cpp
 * @brief CLI subcommand implementation for the audit daemon.
 */

#include "audit/backend/commands/audit_manager.h"

#include "audit/backend/AuditConsumer.h"
#include "audit/backend/AuditWriter.h"

#include <drogon/drogon.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <atomic>
#include <csignal>
#include <fstream>
#include <thread>

namespace
{
std::atomic<bool> g_stop{false};

void onSignal(int) { g_stop.store(true); }

struct AuditConfig
{
    std::string topic  = "audit.events";
    std::string group  = "nextra-audit-manager";
    std::string brokers = "kafka:9092";
};

AuditConfig loadAuditConfig(const std::string& path)
{
    AuditConfig c;
    std::ifstream f(path);
    if (!f) {
        spdlog::warn("audit: missing {}, using defaults", path);
        return c;
    }
    nlohmann::json j;
    f >> j;
    c.topic   = j.value("topic",   c.topic);
    c.group   = j.value("consumerGroup", c.group);
    c.brokers = j.value("brokers", c.brokers);
    return c;
}
}  // namespace

namespace commands
{

void cmdAuditManager(const std::string& config)
{
    std::signal(SIGINT,  onSignal);
    std::signal(SIGTERM, onSignal);

    drogon::app().loadConfigFile(config);
    auto db = drogon::app().getDbClient();

    auto cfg = loadAuditConfig(
        "constants/audit-manager.json");

    nextra::audit::AuditWriter writer(db);
    auto consumer = nextra::audit::makeKafkaConsumer(
        cfg.brokers, cfg.group);

    std::thread httpThread([] { drogon::app().run(); });
    std::thread loop([&] {
        nextra::audit::runConsumerLoop(
            *consumer, writer, cfg.topic, g_stop);
    });

    spdlog::info(
        "audit-manager ready (topic={}, group={})",
        cfg.topic, cfg.group);

    while (!g_stop.load())
        std::this_thread::sleep_for(
            std::chrono::milliseconds{200});

    spdlog::info("audit-manager: shutdown signal received");
    drogon::app().quit();
    if (httpThread.joinable()) httpThread.join();
    if (loop.joinable())       loop.join();
}

}  // namespace commands
