/**
 * @file services/audit/audit_consumer_adapter.cpp
 * @brief Implementation of @ref nextra::audit::InfraConsumerAdapter.
 */

#include "audit/backend/audit_consumer_adapter.h"
#include "infra/backend/KafkaFactory.h"

#include <spdlog/spdlog.h>

#include <utility>

namespace nextra::audit
{

InfraConsumerAdapter::InfraConsumerAdapter(
    const std::string& brokers,
    const std::string& group)
    : brokers_(brokers)
    , group_(group)
{
}

void InfraConsumerAdapter::subscribe(
    const std::string& topic)
{
    inner_ = nextra::infra::makeKafkaConsumer(
        brokers_, group_, topic);
    inner_->setHandler(
        [this](const std::string&,
               const std::string& payload) {
            std::lock_guard<std::mutex> g(mu_);
            queue_.push_back(payload);
        });
    spdlog::info(
        "audit consumer: subscribe({})", topic);
}

int InfraConsumerAdapter::poll(
    std::function<bool(const std::string&)> cb,
    int maxMessages)
{
    if (inner_) inner_->poll(200);
    int handled = 0;
    for (int i = 0; i < maxMessages; ++i) {
        std::string msg;
        {
            std::lock_guard<std::mutex> g(mu_);
            if (queue_.empty()) break;
            msg = std::move(queue_.front());
            queue_.pop_front();
        }
        if (cb(msg)) ++handled;
    }
    if (handled > 0 && inner_) inner_->commit();
    return handled;
}

void InfraConsumerAdapter::close()
{
    if (inner_) inner_->close();
}

}  // namespace nextra::audit
