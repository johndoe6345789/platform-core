#pragma once
/**
 * @file services/infra/IKafkaProducer.h
 * @brief Abstract Kafka producer contract — mirrors the
 *        existing @ref nextra::infra::KafkaProducer shape so
 *        publish sites do not care which implementation is
 *        linked into the binary.
 */

#include <string>

namespace nextra::infra
{

/**
 * @class IKafkaProducer
 * @brief Fire-and-forget producer interface.
 *
 * All production call sites are fire-and-forget — the real
 * implementation enqueues to librdkafka with
 * @c RD_KAFKA_MSG_F_COPY and drains delivery reports from a
 * background poll thread. The stub discards the message and
 * logs at trace level.
 */
class IKafkaProducer
{
  public:
    virtual ~IKafkaProducer() = default;

    /**
     * @brief Publish one message.
     * @param topic Target topic name.
     * @param key   Partition key; may be empty.
     * @param value Payload bytes.
     * @return True if the message was accepted by the
     *         underlying queue (always true for the stub).
     */
    virtual bool produce(
        const std::string& topic,
        const std::string& key,
        const std::string& value) = 0;

    /** @brief Block until all pending messages are delivered. */
    virtual void flush(int timeoutMs) = 0;

    /** @brief True when bound to a live broker connection. */
    virtual bool isConnected() const = 0;
};

} // namespace nextra::infra
