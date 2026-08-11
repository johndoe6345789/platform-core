/**
 * @file services/infra/rdkafka_consumer_config.cpp
 * @brief @c rd_kafka_conf_set helpers for @ref RdKafkaConsumer,
 *        split out so RdKafkaConsumer.cpp stays under the
 *        100-LOC file cap and configuration concerns are
 *        isolated from lifecycle concerns.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/RdKafkaConsumer.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

#include <string>

namespace nextra::infra
{

namespace
{

/**
 * @brief Wrapper around @c rd_kafka_conf_set that logs
 *        failures instead of silently ignoring them.
 * @return True on success, false if rdkafka rejected the kv.
 */
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
            "rd_kafka_conf_set {}={} failed: {}",
            key, value, err);
        return false;
    }
    return true;
}

} // namespace

rd_kafka_conf_t* RdKafkaConsumer::buildConf()
{
    auto* conf = rd_kafka_conf_new();
    bool ok = true;
    ok &= cset(
        conf, "bootstrap.servers", brokers_.c_str());
    ok &= cset(conf, "group.id", groupId_.c_str());
    ok &= cset(conf, "enable.auto.commit", "false");
    ok &= cset(
        conf, "enable.partition.eof", "false");
    ok &= cset(conf, "auto.offset.reset", "earliest");
    ok &= cset(
        conf, "session.timeout.ms", "30000");
    ok &= cset(
        conf, "max.poll.interval.ms", "600000");
    if (!ok) {
        rd_kafka_conf_destroy(conf);
        return nullptr;
    }
    return conf;
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
