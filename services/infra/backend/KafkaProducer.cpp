/**
 * @file KafkaProducer.cpp
 * @brief librdkafka producer body. Stub fallback lives in
 *        kafka_producer_stub.cpp; see KafkaProducer.h.
 */
#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/KafkaProducer.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

#include <cstdlib>

namespace nextra::infra
{

namespace
{
constexpr const char* kDefaultBootstrap = "kafka:9092";
constexpr const char* kClientIdPrefix = "nextra-";

std::string envOr(const char* k, const char* def)
{
    const char* v = std::getenv(k);
    return v ? std::string(v) : std::string(def);
}
} // namespace

KafkaProducer::KafkaProducer(
    const std::string& clientId)
    : clientId_(kClientIdPrefix + clientId)
{
    char err[512];
    auto* conf = rd_kafka_conf_new();
    const auto bs = envOr(
        "KAFKA_BOOTSTRAP_SERVERS", kDefaultBootstrap);
    auto cset = [&](const char* k, const char* v) {
        rd_kafka_conf_set(conf, k, v, err, sizeof(err));
    };
    cset("bootstrap.servers", bs.c_str());
    cset("client.id", clientId_.c_str());
    cset("acks", "all");
    cset("compression.type", "lz4");
    rk_ = rd_kafka_new(
        RD_KAFKA_PRODUCER, conf, err, sizeof(err));
    if (rk_ == nullptr) {
        spdlog::warn("KafkaProducer init: {}", err);
        return;
    }
    spdlog::info("KafkaProducer -> {} ({})", bs, clientId_);
}

KafkaProducer::~KafkaProducer()
{
    if (rk_ == nullptr) return;
    rd_kafka_flush(rk_, 2000);
    rd_kafka_destroy(rk_);
}

bool KafkaProducer::publish(
    const std::string& topic,
    const std::string& key,
    const std::string& value)
{
    if (rk_ == nullptr) return false;
    auto err = rd_kafka_producev(
        rk_,
        RD_KAFKA_V_TOPIC(topic.c_str()),
        RD_KAFKA_V_KEY(key.data(), key.size()),
        RD_KAFKA_V_VALUE(
            const_cast<char*>(value.data()),
            value.size()),
        RD_KAFKA_V_MSGFLAGS(RD_KAFKA_MSG_F_COPY),
        RD_KAFKA_V_END);
    rd_kafka_poll(rk_, 0);
    return err == RD_KAFKA_RESP_ERR_NO_ERROR;
}

void KafkaProducer::flush(int timeoutMs)
{
    if (rk_ != nullptr) {
        rd_kafka_flush(rk_, timeoutMs);
    }
}

bool KafkaProducer::isConnected() const
{
    return rk_ != nullptr;
}

KafkaProducer& KafkaProducer::instance()
{
    static KafkaProducer inst("backend");
    return inst;
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
