/**
 * @file services/infra/kafka_producer_stub.cpp
 * @brief No-op fallback for @ref nextra::infra::KafkaProducer
 *        used when @c NEXTRA_HAVE_KAFKA is not defined.
 *
 * Keeps the legacy @c KafkaProducer::instance() singleton
 * referenced from HealthzController compiling even in builds
 * without librdkafka. For new code prefer the interface-based
 * @ref nextra::infra::makeKafkaProducer factory.
 */

#ifndef NEXTRA_HAVE_KAFKA

#include "infra/backend/KafkaProducer.h"

#include <spdlog/spdlog.h>

namespace nextra::infra
{

KafkaProducer::KafkaProducer(
    const std::string& clientId)
    : clientId_("nextra-" + clientId)
{
    spdlog::info(
        "KafkaProducer stub active (client={})",
        clientId_);
}

KafkaProducer::~KafkaProducer() = default;

bool KafkaProducer::publish(
    const std::string&,
    const std::string&,
    const std::string&)
{
    return false;
}

void KafkaProducer::flush(int) {}

bool KafkaProducer::isConnected() const
{
    return false;
}

KafkaProducer& KafkaProducer::instance()
{
    static KafkaProducer inst("backend");
    return inst;
}

} // namespace nextra::infra

#endif // !NEXTRA_HAVE_KAFKA
