#pragma once
/**
 * @file services/infra/RdKafkaProducer.h
 * @brief Real librdkafka-backed @ref IKafkaProducer.
 *
 * Only compiled when @c NEXTRA_HAVE_KAFKA is defined. Uses
 * @c rd_kafka_producev with @c RD_KAFKA_MSG_F_COPY so callers
 * never need to keep buffers alive. A background thread drains
 * delivery reports so @c rd_kafka_poll is always serviced.
 */

#include "infra/backend/IKafkaProducer.h"

#include <atomic>
#include <string>
#include <thread>

struct rd_kafka_s;

namespace nextra::infra
{

/**
 * @class RdKafkaProducer
 * @brief librdkafka producer wrapper.
 */
class RdKafkaProducer final : public IKafkaProducer
{
  public:
    /**
     * @brief Construct, configure, connect.
     * @param brokers  Comma-separated bootstrap servers.
     * @param clientId Client-id suffix (appended to prefix).
     */
    RdKafkaProducer(
        std::string brokers,
        std::string clientId);

    ~RdKafkaProducer() override;

    RdKafkaProducer(const RdKafkaProducer&) = delete;
    RdKafkaProducer& operator=(
        const RdKafkaProducer&) = delete;

    bool produce(
        const std::string& topic,
        const std::string& key,
        const std::string& value) override;

    void flush(int timeoutMs) override;

    bool isConnected() const override;

  private:
    /** @brief Background loop draining delivery reports. */
    void pollLoop();

    std::string brokers_;
    std::string clientId_;
    rd_kafka_s* rk_{nullptr};
    std::atomic<bool> running_{false};
    std::thread pollThread_;
};

} // namespace nextra::infra
