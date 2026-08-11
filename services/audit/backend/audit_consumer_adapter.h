#pragma once
/**
 * @file services/audit/audit_consumer_adapter.h
 * @brief Adapter wrapping @ref nextra::infra::IKafkaConsumer so
 *        audit code keeps its own @ref nextra::audit::IKafkaConsumer
 *        shape (poll(cb, max) returning dispatched count). Split
 *        out of AuditConsumer.cpp to respect the 100-LOC cap.
 */

#include "audit/backend/AuditConsumer.h"
#include "infra/backend/IKafkaConsumer.h"

#include <deque>
#include <memory>
#include <mutex>
#include <string>

namespace nextra::audit
{

/**
 * @class InfraConsumerAdapter
 * @brief Converts the audit-local poll-with-callback API into
 *        the unified infra handler-push model by buffering
 *        incoming messages in a small in-memory queue.
 */
class InfraConsumerAdapter : public IKafkaConsumer
{
public:
    InfraConsumerAdapter(
        const std::string& brokers,
        const std::string& group);

    void subscribe(const std::string& topic) override;

    int poll(std::function<bool(const std::string&)> cb,
             int maxMessages) override;

    void close() override;

private:
    std::unique_ptr<nextra::infra::IKafkaConsumer> inner_;
    std::deque<std::string> queue_;
    std::mutex mu_;
    std::string brokers_;
    std::string group_;
};

}  // namespace nextra::audit
