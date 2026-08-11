#pragma once
/**
 * @file services/infra/StubKafkaConsumer.h
 * @brief No-op @ref IKafkaConsumer used when librdkafka is
 *        not linked into the build (@c NEXTRA_HAVE_KAFKA
 *        undefined).
 *
 * Header-only so it is always available regardless of whether
 * the rdkafka-backed translation units made it into the build.
 * Daemons using this path simply idle and the REST/HTTP surface
 * remains available for smoke testing.
 */

#include "infra/backend/IKafkaConsumer.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <string>
#include <thread>
#include <utility>
#include <vector>

namespace nextra::infra
{

/**
 * @class StubKafkaConsumer
 * @brief Idle-loop implementation of @ref IKafkaConsumer.
 */
class StubKafkaConsumer final : public IKafkaConsumer
{
  public:
    /**
     * @brief Construct a stub with a cosmetic identity.
     * @param groupId Group id, only used in log lines.
     * @param topics  Topic names, only used in log lines.
     */
    StubKafkaConsumer(
        std::string groupId,
        std::vector<std::string> topics)
        : groupId_(std::move(groupId))
        , topics_(std::move(topics))
    {
        spdlog::info(
            "StubKafkaConsumer group={} topics={} "
            "(librdkafka not linked)",
            groupId_, topics_.size());
    }

    void setHandler(KafkaMessageHandler h) override
    {
        handler_ = std::move(h);
    }

    int poll(int timeoutMs) override
    {
        std::this_thread::sleep_for(
            std::chrono::milliseconds{timeoutMs});
        return 0;
    }

    void commit() override {}

    void close() override
    {
        spdlog::info(
            "StubKafkaConsumer {} closed", groupId_);
    }

    bool isConnected() const override { return false; }

  private:
    std::string groupId_;
    std::vector<std::string> topics_;
    KafkaMessageHandler handler_;
};

} // namespace nextra::infra
