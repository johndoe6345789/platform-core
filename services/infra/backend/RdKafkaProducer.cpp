/**
 * @file services/infra/RdKafkaProducer.cpp
 * @brief Construction, teardown and delivery-report loop for
 *        @ref nextra::infra::RdKafkaProducer. Config helpers
 *        live in rdkafka_producer_config.cpp and the
 *        produce/flush surface lives in
 *        rdkafka_producer_publish.cpp so each file stays under
 *        the 100-LOC cap.
 */

#ifdef NEXTRA_HAVE_KAFKA

#include "infra/backend/RdKafkaProducer.h"
#include "infra/backend/rdkafka_producer_internal.h"

#include <librdkafka/rdkafka.h>
#include <spdlog/spdlog.h>

#include <utility>

namespace nextra::infra
{

using producer_detail::cset;
using producer_detail::drCallback;

RdKafkaProducer::RdKafkaProducer(
    std::string brokers,
    std::string clientId)
    : brokers_(std::move(brokers))
    , clientId_(std::move(clientId))
{
    auto* conf = rd_kafka_conf_new();
    bool ok = true;
    ok &= cset(
        conf, "bootstrap.servers", brokers_.c_str());
    ok &= cset(conf, "client.id", clientId_.c_str());
    ok &= cset(conf, "acks", "all");
    ok &= cset(conf, "compression.type", "lz4");
    ok &= cset(conf, "enable.idempotence", "true");
    if (!ok) {
        rd_kafka_conf_destroy(conf);
        return;
    }
    rd_kafka_conf_set_dr_msg_cb(conf, &drCallback);

    char err[512];
    rk_ = rd_kafka_new(
        RD_KAFKA_PRODUCER, conf, err, sizeof(err));
    if (rk_ == nullptr) {
        spdlog::error(
            "RdKafkaProducer new failed: {}", err);
        return;
    }
    running_.store(true);
    pollThread_ = std::thread(
        &RdKafkaProducer::pollLoop, this);
    spdlog::info(
        "RdKafkaProducer ready client={} brokers={}",
        clientId_, brokers_);
}

RdKafkaProducer::~RdKafkaProducer()
{
    running_.store(false);
    if (pollThread_.joinable()) pollThread_.join();
    if (rk_ != nullptr) {
        rd_kafka_flush(rk_, 2000);
        rd_kafka_destroy(rk_);
        rk_ = nullptr;
    }
}

void RdKafkaProducer::pollLoop()
{
    while (running_.load()) {
        rd_kafka_poll(rk_, 200);
    }
}

} // namespace nextra::infra

#endif // NEXTRA_HAVE_KAFKA
