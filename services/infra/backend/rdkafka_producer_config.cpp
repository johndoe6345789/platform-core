/**
 * @file services/infra/rdkafka_producer_config.cpp
 * @brief conf_set helpers and delivery-report callback for
 *        @ref nextra::infra::RdKafkaProducer. Split out so
 *        RdKafkaProducer.cpp stays under the 100-LOC cap.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/rdkafka_producer_internal.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

namespace nextra::infra::producer_detail
{

bool cset(
    rd_kafka_conf_t* conf,
    const char* key,
    const char* value)
{
    char err[512];
    const auto rc = rd_kafka_conf_set(
        conf, key, value, err, sizeof(err));
    if (rc != RD_KAFKA_CONF_OK) {
        spdlog::error(
            "producer conf_set {}={} failed: {}",
            key, value, err);
        return false;
    }
    return true;
}

void drCallback(
    rd_kafka_t*,
    const rd_kafka_message_t* msg,
    void*)
{
    if (msg->err != RD_KAFKA_RESP_ERR_NO_ERROR) {
        spdlog::warn(
            "kafka delivery failed: {}",
            rd_kafka_err2str(msg->err));
    }
}

} // namespace nextra::infra::producer_detail

#endif // NEXTRA_HAVE_KAFKA
