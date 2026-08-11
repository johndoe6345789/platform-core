#pragma once
/**
 * @file services/infra/RdKafkaConsumer.h
 * @brief Real librdkafka-backed @ref IKafkaConsumer.
 *
 * Only compiled when @c NEXTRA_HAVE_KAFKA is defined (i.e.
 * @c find_package(RdKafka) succeeded). Daemons should never
 * instantiate this directly — go through
 * @ref makeKafkaConsumer in KafkaFactory.h which will return
 * the stub when librdkafka is absent.
 *
 * Construction is split across three translation units to stay
 * under the 100-LOC file cap:
 *   - RdKafkaConsumer.cpp           : ctor / dtor / close
 *   - rdkafka_consumer_config.cpp   : conf_set helpers
 *   - rdkafka_consumer_poll.cpp     : poll / commit
 */

#include "infra/backend/IKafkaConsumer.h"

#include <string>
#include <vector>

struct rd_kafka_s;
struct rd_kafka_conf_s;

namespace nextra::infra
{

/**
 * @class RdKafkaConsumer
 * @brief Thin C++ wrapper around @c rd_kafka_consumer_poll.
 */
class RdKafkaConsumer final : public IKafkaConsumer
{
  public:
    /**
     * @brief Build, configure and subscribe a consumer.
     * @param brokers Comma-separated bootstrap servers.
     * @param groupId Consumer group id.
     * @param topics  Topic names to subscribe to.
     */
    RdKafkaConsumer(
        std::string brokers,
        std::string groupId,
        std::vector<std::string> topics);

    ~RdKafkaConsumer() override;

    RdKafkaConsumer(const RdKafkaConsumer&) = delete;
    RdKafkaConsumer& operator=(
        const RdKafkaConsumer&) = delete;

    void setHandler(KafkaMessageHandler h) override;
    int poll(int timeoutMs) override;
    void commit() override;
    void close() override;
    bool isConnected() const override;

  private:
    /** @brief Build the conf object; logs and returns null on
     *         any conf_set error. */
    ::rd_kafka_conf_s* buildConf();

    /** @brief Subscribe to @ref topics_; logs errors. */
    void subscribeTopics();

    std::string brokers_;
    std::string groupId_;
    std::vector<std::string> topics_;
    KafkaMessageHandler handler_;
    rd_kafka_s* rk_{nullptr};
    bool closed_{false};
};

} // namespace nextra::infra
