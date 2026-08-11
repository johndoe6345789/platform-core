#pragma once

/**
 * @file AuditConsumer.h
 * @brief Abstract Kafka consumer for the `audit.events` topic.
 *
 * The real librdkafka wrapper lives behind `NEXTRA_HAVE_KAFKA`
 * while Phase 0 infra is still in flight.  Until it is defined,
 * `StubKafkaConsumer` is used — it returns no messages and the
 * audit-manager daemon still boots cleanly so it can be smoke
 * tested.  When the real client lands, a single #define flip
 * swaps the implementation without touching the controller.
 */

#include "AuditTypes.h"

#include <functional>
#include <memory>
#include <string>

namespace nextra::audit
{

/** @brief Producer-agnostic Kafka consumer contract. */
class IKafkaConsumer
{
public:
    virtual ~IKafkaConsumer() = default;

    /** @brief Subscribe to a topic.  Idempotent. */
    virtual void subscribe(const std::string& topic) = 0;

    /**
     * @brief Poll once; invoke cb for each message received.
     * @param cb Handler; return true to commit the offset.
     * @param maxMessages Upper bound for this poll.
     * @return Number of messages handed to `cb`.
     */
    virtual int poll(
        std::function<bool(const std::string&)> cb,
        int maxMessages) = 0;

    /** @brief Graceful close; commits pending offsets. */
    virtual void close() = 0;
};

/** @brief Build the compiled-in consumer (stub or real). */
std::unique_ptr<IKafkaConsumer>
makeKafkaConsumer(const std::string& brokers,
                  const std::string& group);

/**
 * @brief Run the consume → decode → write loop.
 *
 * Runs until `stop` is set.  Each decoded AuditEvent is handed to
 * the writer; a failed decode logs and continues (the offset is
 * still committed so we never hot-loop on poison messages — the
 * dead-letter topic is introduced in Phase 1.3).
 *
 * @param consumer Live Kafka consumer.
 * @param writer   Drogon writer.
 * @param topic    Topic to subscribe to.
 * @param stop     Atomic stop flag set by the daemon on SIGTERM.
 */
void runConsumerLoop(IKafkaConsumer& consumer,
                     class AuditWriter& writer,
                     const std::string& topic,
                     std::atomic<bool>& stop);

}  // namespace nextra::audit
