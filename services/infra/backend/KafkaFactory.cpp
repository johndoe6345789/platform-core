/**
 * @file services/infra/KafkaFactory.cpp
 * @brief Implementation of @ref nextra::infra::makeKafkaConsumer
 *        and friends. Uses preprocessor selection so the stub
 *        translation unit still compiles without librdkafka.
 */

#include "infra/backend/KafkaFactory.h"
#include "infra/backend/StubKafkaConsumer.h"
#include "infra/backend/StubKafkaProducer.h"

#ifdef NEXTRA_HAVE_KAFKA
#include "infra/backend/RdKafkaConsumer.h"
#include "infra/backend/RdKafkaProducer.h"
#endif

#include <cstdlib>

namespace nextra::infra
{

namespace
{
constexpr const char* kDefaultBootstrap = "kafka:9092";

std::string resolveBrokers(const std::string& explicitValue)
{
    if (!explicitValue.empty()) return explicitValue;
    if (const char* env =
            std::getenv("KAFKA_BOOTSTRAP_SERVERS")) {
        return std::string(env);
    }
    return std::string(kDefaultBootstrap);
}
} // namespace

bool hasRealKafka()
{
#ifdef NEXTRA_HAVE_KAFKA
    return true;
#else
    return false;
#endif
}

std::unique_ptr<IKafkaConsumer> makeKafkaConsumer(
    const std::string& brokers,
    const std::string& groupId,
    const std::vector<std::string>& topics)
{
    const auto bs = resolveBrokers(brokers);
#ifdef NEXTRA_HAVE_KAFKA
    return std::make_unique<RdKafkaConsumer>(
        bs, groupId, topics);
#else
    (void)bs;
    return std::make_unique<StubKafkaConsumer>(
        groupId, topics);
#endif
}

std::unique_ptr<IKafkaConsumer> makeKafkaConsumer(
    const std::string& brokers,
    const std::string& groupId,
    const std::string& topic)
{
    return makeKafkaConsumer(
        brokers, groupId,
        std::vector<std::string>{topic});
}

std::unique_ptr<IKafkaProducer> makeKafkaProducer(
    const std::string& brokers,
    const std::string& clientId)
{
    const auto bs = resolveBrokers(brokers);
#ifdef NEXTRA_HAVE_KAFKA
    return std::make_unique<RdKafkaProducer>(
        bs, clientId);
#else
    (void)bs;
    (void)clientId;
    return std::make_unique<StubKafkaProducer>();
#endif
}

} // namespace nextra::infra
