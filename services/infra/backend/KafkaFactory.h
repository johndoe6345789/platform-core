#pragma once
/**
 * @file services/infra/KafkaFactory.h
 * @brief Compile-time selection between the real librdkafka
 *        consumer/producer and the header-only stubs.
 *
 * Daemons never reach for @c RdKafkaConsumer or
 * @c StubKafkaConsumer by name — they call
 * @ref makeKafkaConsumer and @ref makeKafkaProducer and get the
 * right implementation for the current build. When librdkafka
 * is unavailable (@c NEXTRA_HAVE_KAFKA undefined) the stubs are
 * returned and the daemon still boots cleanly for smoke tests.
 */

#include "infra/backend/IKafkaConsumer.h"
#include "infra/backend/IKafkaProducer.h"

#include <memory>
#include <string>
#include <vector>

namespace nextra::infra
{

/**
 * @brief Build the compiled-in consumer (real or stub).
 * @param brokers Comma-separated bootstrap servers; falls back
 *                to @c KAFKA_BOOTSTRAP_SERVERS env when empty.
 * @param groupId Consumer group id.
 * @param topics  Topic names to subscribe to.
 */
std::unique_ptr<IKafkaConsumer> makeKafkaConsumer(
    const std::string& brokers,
    const std::string& groupId,
    const std::vector<std::string>& topics);

/**
 * @brief Convenience overload for the common single-topic case.
 */
std::unique_ptr<IKafkaConsumer> makeKafkaConsumer(
    const std::string& brokers,
    const std::string& groupId,
    const std::string& topic);

/**
 * @brief Build the compiled-in producer (real or stub).
 * @param brokers  Comma-separated bootstrap servers; falls back
 *                 to @c KAFKA_BOOTSTRAP_SERVERS env when empty.
 * @param clientId Client-id suffix.
 */
std::unique_ptr<IKafkaProducer> makeKafkaProducer(
    const std::string& brokers,
    const std::string& clientId);

/** @brief True if this binary was compiled with librdkafka. */
bool hasRealKafka();

} // namespace nextra::infra
