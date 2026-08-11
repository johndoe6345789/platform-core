/**
 * @file JobBackoff.cpp
 * @brief Retry-delay calculation for the job scheduler.
 */

#include "JobBackoff.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace nextra::jobs
{

namespace
{
/// Thread-local PRNG used for the ±15% jitter on exponential backoff.
std::mt19937& prng()
{
    thread_local std::mt19937 gen{std::random_device{}()};
    return gen;
}

std::chrono::milliseconds clamp(std::chrono::milliseconds d, const BackoffConfig& cfg)
{
    return std::min(std::max(d, std::chrono::milliseconds{1}), cfg.maxDelay);
}
}  // namespace

std::chrono::milliseconds computeBackoff(BackoffStrategy strategy,
                                         int attempt,
                                         const BackoffConfig& cfg)
{
    const int a = std::max(1, attempt);
    switch (strategy)
    {
        case BackoffStrategy::Fixed:
            return clamp(cfg.baseDelay, cfg);

        case BackoffStrategy::Linear:
            return clamp(cfg.baseDelay * a, cfg);

        case BackoffStrategy::Exponential:
        {
            // base * 2^(attempt-1)
            const auto expMs = cfg.baseDelay.count() * (1LL << std::min(a - 1, 30));
            std::uniform_real_distribution<double> jitter(0.85, 1.15);
            const auto jittered = static_cast<long long>(expMs * jitter(prng()));
            return clamp(std::chrono::milliseconds{jittered}, cfg);
        }
    }
    return clamp(cfg.baseDelay, cfg);
}

}  // namespace nextra::jobs
