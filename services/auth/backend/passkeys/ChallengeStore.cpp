/**
 * @file ChallengeStore.cpp
 * @brief Implementation of the in-memory challenge store.
 */

#include "ChallengeStore.h"

#include <chrono>

namespace services::auth::passkeys
{

static std::int64_t nowSec()
{
    using namespace std::chrono;
    return duration_cast<seconds>(
        system_clock::now().time_since_epoch()).count();
}

ChallengeStore& ChallengeStore::instance()
{
    static ChallengeStore inst;
    return inst;
}

void ChallengeStore::put(
    const std::string& key, const PendingChallenge& value)
{
    std::lock_guard lk(mu_);
    auto copy = value;
    copy.createdAtSec = nowSec();
    map_[key] = std::move(copy);
}

std::optional<PendingChallenge> ChallengeStore::consume(
    const std::string& key)
{
    std::lock_guard lk(mu_);
    auto it = map_.find(key);
    if (it == map_.end()) {
        return std::nullopt;
    }
    auto val = it->second;
    map_.erase(it);
    return val;
}

void ChallengeStore::sweep(int ttlSeconds)
{
    std::lock_guard lk(mu_);
    const auto cutoff = nowSec() - ttlSeconds;
    for (auto it = map_.begin(); it != map_.end();) {
        if (it->second.createdAtSec < cutoff) {
            it = map_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace services::auth::passkeys
