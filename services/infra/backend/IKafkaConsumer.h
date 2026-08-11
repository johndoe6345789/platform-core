#pragma once
/**
 * @file services/infra/IKafkaConsumer.h
 * @brief Unified Kafka consumer interface used by every daemon.
 *
 * Prior to Phase 0.4 each daemon defined its own incompatible
 * `IKafkaConsumer` shape (audit, search, notification-router).
 * This header is the single source of truth: daemons talk to the
 * interface, @ref nextra::infra::makeKafkaConsumer picks the real
 * librdkafka implementation when @c NEXTRA_HAVE_KAFKA is defined
 * and falls back to @ref StubKafkaConsumer otherwise.
 */

#include <functional>
#include <string>
#include <vector>

namespace nextra::infra
{

/**
 * @brief Handler invoked for every received message.
 * @param key     Message key (empty if none).
 * @param payload Message payload bytes.
 */
using KafkaMessageHandler = std::function<void(
    const std::string& key,
    const std::string& payload)>;

/**
 * @class IKafkaConsumer
 * @brief Abstract Kafka consumer contract.
 *
 * Consumers are constructed already bound to a broker list,
 * group id and topic set. @ref poll drains messages up to the
 * given timeout, dispatching each one to the handler registered
 * via @ref setHandler. @ref commit persists offsets; the stub
 * path is a no-op.
 */
class IKafkaConsumer
{
  public:
    virtual ~IKafkaConsumer() = default;

    /** @brief Register the per-message handler. */
    virtual void setHandler(KafkaMessageHandler h) = 0;

    /**
     * @brief Drain pending messages.
     * @param timeoutMs Wait up to this many ms for one message.
     * @return Number of messages dispatched this call.
     */
    virtual int poll(int timeoutMs) = 0;

    /** @brief Commit current offsets synchronously. */
    virtual void commit() = 0;

    /** @brief Close the consumer; idempotent. */
    virtual void close() = 0;

    /** @brief True when bound to a live broker connection. */
    virtual bool isConnected() const = 0;
};

} // namespace nextra::infra
