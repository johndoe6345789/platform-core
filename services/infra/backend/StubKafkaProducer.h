#pragma once
/**
 * @file services/infra/StubKafkaProducer.h
 * @brief No-op @ref IKafkaProducer used when librdkafka is
 *        not linked into the build (@c NEXTRA_HAVE_KAFKA
 *        undefined). Header-only so it always compiles.
 */

#include "infra/backend/IKafkaProducer.h"

#include <spdlog/spdlog.h>

#include <string>

namespace nextra::infra
{

/**
 * @class StubKafkaProducer
 * @brief Discards every message, logs at trace level.
 */
class StubKafkaProducer final : public IKafkaProducer
{
  public:
    StubKafkaProducer()
    {
        spdlog::info(
            "StubKafkaProducer active "
            "(librdkafka not linked)");
    }

    bool produce(
        const std::string& topic,
        const std::string& key,
        const std::string& value) override
    {
        spdlog::trace(
            "stub produce topic={} keyLen={} valLen={}",
            topic, key.size(), value.size());
        return true;
    }

    void flush(int) override {}

    bool isConnected() const override { return false; }
};

} // namespace nextra::infra
