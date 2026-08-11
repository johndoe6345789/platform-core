#pragma once
/**
 * @file KafkaProducer.h
 * @brief Thin wrapper around librdkafka's producer.
 *
 * Choice: plain librdkafka (C API) over modern-cpp-kafka.
 * Rationale: librdkafka is the canonical Conan package
 * (`librdkafka/2.3.0`) and is the basis for every C++ wrapper.
 * modern-cpp-kafka's Conan recipe lags upstream. Keeping the
 * dependency list short and direct is preferable for this
 * template. A thin C++ facade replaces the wrapper.
 *
 * Bootstrap servers come from KAFKA_BOOTSTRAP_SERVERS env
 * (default: constants/infra.json -> kafka.default_bootstrap).
 */

#include <cstdint>
#include <memory>
#include <string>

struct rd_kafka_s;
struct rd_kafka_conf_s;

namespace nextra::infra
{

/**
 * @class KafkaProducer
 * @brief Minimal synchronous publish wrapper.
 */
class KafkaProducer
{
  public:
    /**
     * @brief Construct and connect a producer.
     * @param clientId Client-id suffix appended to prefix.
     */
    explicit KafkaProducer(
        const std::string& clientId);
    ~KafkaProducer();

    KafkaProducer(const KafkaProducer&) = delete;
    KafkaProducer& operator=(
        const KafkaProducer&) = delete;

    /**
     * @brief Publish a message to a topic.
     * @param topic Kafka topic name.
     * @param key   Partition key (may be empty).
     * @param value Message payload.
     * @return True if the produce call was enqueued.
     */
    bool publish(
        const std::string& topic,
        const std::string& key,
        const std::string& value);

    /** @brief Flush pending messages, up to timeoutMs. */
    void flush(int timeoutMs = 5000);

    /** @brief True if the producer handle is live. */
    bool isConnected() const;

    /** @brief Singleton accessor for shared producer. */
    static KafkaProducer& instance();

  private:
    rd_kafka_s* rk_ = nullptr;
    std::string clientId_;
};

} // namespace nextra::infra
