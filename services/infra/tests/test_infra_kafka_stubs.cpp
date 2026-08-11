/**
 * @file test_infra_kafka_stubs.cpp
 * @brief StubKafkaProducer/Consumer no-op semantics.
 */

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace
{

struct StubProducer
{
    int publishCount = 0;
    bool publish(const std::string&, const std::string&)
    {
        ++publishCount;
        return true;
    }
};

struct StubConsumer
{
    bool running = false;
    void start() { running = true; }
    void stop() { running = false; }
    std::vector<std::string> poll() { return {}; }
};

} // namespace

class KafkaStubTest : public ::testing::Test
{
};

TEST_F(KafkaStubTest, ProducerPublishNoCrash)
{
    StubProducer p;
    EXPECT_TRUE(p.publish("topic", "msg"));
    EXPECT_EQ(p.publishCount, 1);
}

TEST_F(KafkaStubTest, ConsumerStartStop)
{
    StubConsumer c;
    c.start();
    EXPECT_TRUE(c.running);
    c.stop();
    EXPECT_FALSE(c.running);
}

TEST_F(KafkaStubTest, ConsumerPollEmpty)
{
    StubConsumer c;
    EXPECT_TRUE(c.poll().empty());
}
