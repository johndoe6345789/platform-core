/**
 * @file KafkaConsumer.cpp
 * @brief librdkafka consumer construction + subscribe.
 * Polling loop lives in kafka_consumer_run.cpp.
 *
 * This legacy wrapper is only compiled when @c NEXTRA_HAVE_KAFKA
 * is defined. New code should use @ref nextra::infra::IKafkaConsumer
 * via @ref nextra::infra::makeKafkaConsumer.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/KafkaConsumer.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

#include <cstdlib>

namespace nextra::infra
{

namespace
{
constexpr const char* kDefaultBootstrap = "kafka:9092";

std::string envOr(const char* k, const char* def)
{
    const char* v = std::getenv(k);
    return v ? std::string(v) : std::string(def);
}
} // namespace

KafkaConsumer::KafkaConsumer(
    const std::string& topic,
    const std::string& groupId,
    KafkaHandler handler)
    : topic_(topic)
    , groupId_(groupId)
    , handler_(std::move(handler))
{
    char err[512];
    auto* conf = rd_kafka_conf_new();
    const auto bs = envOr(
        "KAFKA_BOOTSTRAP_SERVERS", kDefaultBootstrap);
    auto cset = [&](const char* k, const char* v) {
        rd_kafka_conf_set(conf, k, v, err, sizeof(err));
    };
    cset("bootstrap.servers", bs.c_str());
    cset("group.id", groupId_.c_str());
    cset("enable.auto.commit", "true");
    cset("auto.offset.reset", "earliest");

    rk_ = rd_kafka_new(
        RD_KAFKA_CONSUMER, conf, err, sizeof(err));
    if (rk_ == nullptr) {
        spdlog::warn(
            "KafkaConsumer init failed: {}", err);
        return;
    }
    rd_kafka_poll_set_consumer(rk_);

    auto* parts = rd_kafka_topic_partition_list_new(1);
    rd_kafka_topic_partition_list_add(
        parts, topic_.c_str(), RD_KAFKA_PARTITION_UA);
    rd_kafka_subscribe(rk_, parts);
    rd_kafka_topic_partition_list_destroy(parts);

    spdlog::info(
        "KafkaConsumer {} -> {} ({})",
        groupId_, topic_, bs);
}

KafkaConsumer::~KafkaConsumer()
{
    stop();
    if (rk_ != nullptr) {
        rd_kafka_consumer_close(rk_);
        rd_kafka_destroy(rk_);
    }
}

bool KafkaConsumer::isConnected() const
{
    return rk_ != nullptr;
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
