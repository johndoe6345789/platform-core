/**
 * @file services/infra/rdkafka_consumer_poll.cpp
 * @brief Poll and commit implementation for
 *        @ref nextra::infra::RdKafkaConsumer.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/RdKafkaConsumer.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

#include <string>

namespace nextra::infra
{

int RdKafkaConsumer::poll(int timeoutMs)
{
    if (rk_ == nullptr) return 0;
    auto* msg = rd_kafka_consumer_poll(rk_, timeoutMs);
    if (msg == nullptr) return 0;
    int dispatched = 0;
    if (msg->err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        if (msg->err
            != RD_KAFKA_RESP_ERR__PARTITION_EOF) {
            spdlog::warn(
                "RdKafkaConsumer {} poll err: {}",
                groupId_,
                rd_kafka_err2str(msg->err));
        }
        rd_kafka_message_destroy(msg);
        return 0;
    }
    std::string key;
    if (msg->key != nullptr && msg->key_len > 0) {
        key.assign(
            static_cast<const char*>(msg->key),
            msg->key_len);
    }
    std::string payload;
    if (msg->payload != nullptr && msg->len > 0) {
        payload.assign(
            static_cast<const char*>(msg->payload),
            msg->len);
    }
    if (handler_) {
        try {
            handler_(key, payload);
            ++dispatched;
        } catch (const std::exception& ex) {
            spdlog::error(
                "RdKafkaConsumer handler threw: {}",
                ex.what());
        }
    }
    rd_kafka_message_destroy(msg);
    return dispatched;
}

void RdKafkaConsumer::commit()
{
    if (rk_ == nullptr) return;
    const auto err = rd_kafka_commit(rk_, nullptr, 0);
    if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        spdlog::warn(
            "RdKafkaConsumer {} commit err: {}",
            groupId_,
            rd_kafka_err2str(err));
    }
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
