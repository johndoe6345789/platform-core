#pragma once
/**
 * @file services/infra/rdkafka_producer_internal.h
 * @brief Private helpers shared between the split
 *        RdKafkaProducer translation units. Not part of the
 *        public API — do not include from anywhere outside
 *        services/infra.
 */

#ifdef NEXTRA_HAVE_KAFKA

struct rd_kafka_conf_s;
struct rd_kafka_s;
struct rd_kafka_message_s;

namespace nextra::infra::producer_detail
{

/**
 * @brief @c rd_kafka_conf_set with @c spdlog logging on any
 *        error.
 * @return True on success, false if rdkafka rejected the kv.
 */
bool cset(
    rd_kafka_conf_s* conf,
    const char* key,
    const char* value);

/** @brief Delivery-report callback logging failed deliveries. */
void drCallback(
    rd_kafka_s* rk,
    const rd_kafka_message_s* msg,
    void* opaque);

} // namespace nextra::infra::producer_detail

#endif // NEXTRA_HAVE_KAFKA
