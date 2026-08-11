/**
 * @file services/infra/RdKafkaConsumer.cpp
 * @brief Construction, subscribe and teardown for the real
 *        librdkafka-backed @ref IKafkaConsumer. Poll / commit
 *        live in rdkafka_consumer_poll.cpp.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/RdKafkaConsumer.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

#include <utility>

namespace nextra::infra
{

RdKafkaConsumer::RdKafkaConsumer(
    std::string brokers,
    std::string groupId,
    std::vector<std::string> topics)
    : brokers_(std::move(brokers))
    , groupId_(std::move(groupId))
    , topics_(std::move(topics))
{
    auto* conf = buildConf();
    if (conf == nullptr) {
        return;
    }
    char err[512];
    rk_ = rd_kafka_new(
        RD_KAFKA_CONSUMER, conf, err, sizeof(err));
    if (rk_ == nullptr) {
        spdlog::error(
            "RdKafkaConsumer rd_kafka_new failed: {}",
            err);
        return;
    }
    rd_kafka_poll_set_consumer(rk_);
    subscribeTopics();
    spdlog::info(
        "RdKafkaConsumer group={} topics={} brokers={}",
        groupId_, topics_.size(), brokers_);
}

RdKafkaConsumer::~RdKafkaConsumer()
{
    close();
}

void RdKafkaConsumer::subscribeTopics()
{
    auto* parts = rd_kafka_topic_partition_list_new(
        static_cast<int>(topics_.size()));
    for (const auto& t : topics_) {
        rd_kafka_topic_partition_list_add(
            parts, t.c_str(), RD_KAFKA_PARTITION_UA);
    }
    const auto err = rd_kafka_subscribe(rk_, parts);
    if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        spdlog::error(
            "RdKafkaConsumer subscribe failed: {}",
            rd_kafka_err2str(err));
    }
    rd_kafka_topic_partition_list_destroy(parts);
}

void RdKafkaConsumer::setHandler(KafkaMessageHandler h)
{
    handler_ = std::move(h);
}

void RdKafkaConsumer::close()
{
    if (closed_) return;
    closed_ = true;
    if (rk_ == nullptr) return;
    rd_kafka_consumer_close(rk_);
    rd_kafka_destroy(rk_);
    rk_ = nullptr;
    spdlog::info(
        "RdKafkaConsumer {} closed", groupId_);
}

bool RdKafkaConsumer::isConnected() const
{
    return rk_ != nullptr;
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
