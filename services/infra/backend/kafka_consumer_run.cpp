/**
 * @file kafka_consumer_run.cpp
 * @brief KafkaConsumer start/stop + polling loop.
 *
 * Split from KafkaConsumer.cpp to stay under the 100-LOC cap.
 * Only compiled when @c NEXTRA_HAVE_KAFKA is defined.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/KafkaConsumer.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

#include <string>

namespace nextra::infra
{

void KafkaConsumer::start()
{
    if (rk_ == nullptr || running_.load()) {
        return;
    }
    running_.store(true);
    worker_ = std::thread(&KafkaConsumer::run, this);
}

void KafkaConsumer::stop()
{
    running_.store(false);
    if (worker_.joinable()) {
        worker_.join();
    }
}

void KafkaConsumer::run()
{
    while (running_.load()) {
        auto* msg = rd_kafka_consumer_poll(rk_, 500);
        if (msg == nullptr) {
            continue;
        }
        if (msg->err != 0) {
            if (msg->err
                != RD_KAFKA_RESP_ERR__PARTITION_EOF) {
                spdlog::warn(
                    "KafkaConsumer {} err: {}",
                    topic_,
                    rd_kafka_err2str(msg->err));
            }
            rd_kafka_message_destroy(msg);
            continue;
        }
        std::string key(
            static_cast<const char*>(msg->key),
            msg->key_len);
        std::string value(
            static_cast<const char*>(msg->payload),
            msg->len);
        try {
            handler_(key, value);
        } catch (const std::exception& e) {
            spdlog::error(
                "KafkaConsumer handler threw: {}",
                e.what());
        }
        rd_kafka_message_destroy(msg);
    }
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
