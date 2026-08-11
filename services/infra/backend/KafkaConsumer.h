#pragma once
/**
 * @file KafkaConsumer.h
 * @brief librdkafka consumer wrapper with a handler thread.
 */

#include <atomic>
#include <functional>
#include <string>
#include <thread>

struct rd_kafka_s;

namespace nextra::infra
{

/**
 * @brief Callback invoked per consumed message.
 * @param key   Message key (may be empty).
 * @param value Message payload.
 */
using KafkaHandler = std::function<void(
    const std::string& key,
    const std::string& value)>;

/**
 * @class KafkaConsumer
 * @brief Subscribes to a topic and runs a handler thread.
 */
class KafkaConsumer
{
  public:
    /**
     * @brief Build a consumer and subscribe.
     * @param topic   Topic to subscribe to.
     * @param groupId Consumer group id.
     * @param handler Invoked for every message.
     */
    KafkaConsumer(
        const std::string& topic,
        const std::string& groupId,
        KafkaHandler handler);

    ~KafkaConsumer();

    KafkaConsumer(const KafkaConsumer&) = delete;
    KafkaConsumer& operator=(
        const KafkaConsumer&) = delete;

    /** @brief Start the background polling thread. */
    void start();

    /** @brief Stop polling and join the thread. */
    void stop();

    /** @brief True if the consumer handle is live. */
    bool isConnected() const;

  private:
    void run();

    rd_kafka_s* rk_ = nullptr;
    std::string topic_;
    std::string groupId_;
    KafkaHandler handler_;
    std::atomic<bool> running_{false};
    std::thread worker_;
};

} // namespace nextra::infra
