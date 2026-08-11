/**
 * @file services/infra/rdkafka_producer_publish.cpp
 * @brief Synchronous produce / flush / isConnected surface of
 *        @ref nextra::infra::RdKafkaProducer. Split out from
 *        RdKafkaProducer.cpp so each translation unit stays
 *        under the 100-LOC cap.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/RdKafkaProducer.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

namespace nextra::infra
{

bool RdKafkaProducer::produce(
    const std::string& topic,
    const std::string& key,
    const std::string& value)
{
    if (rk_ == nullptr) return false;
    const auto err = rd_kafka_producev(
        rk_,
        RD_KAFKA_V_TOPIC(topic.c_str()),
        RD_KAFKA_V_KEY(key.data(), key.size()),
        RD_KAFKA_V_VALUE(
            const_cast<char*>(value.data()),
            value.size()),
        RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
        RD_KAFKA_V_END);
    if (err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        spdlog::warn(
            "RdKafkaProducer produce topic={} err={}",
            topic, rd_kafka_err2str(err));
        return false;
    }
    return true;
}

void RdKafkaProducer::flush(int timeoutMs)
{
    if (rk_ != nullptr) {
        rd_kafka_flush(rk_, timeoutMs);
    }
}

bool RdKafkaProducer::isConnected() const
{
    return rk_ != nullptr;
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
