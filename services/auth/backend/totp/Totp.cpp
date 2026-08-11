/**
 * @file Totp.cpp
 * @brief TOTP verification implementation.
 */

#include "Totp.h"
#include "Hotp.h"

#include <chrono>

namespace services::auth::totp
{

static std::uint64_t nowStep(int stepSec)
{
    using namespace std::chrono;
    auto s = duration_cast<seconds>(
        system_clock::now().time_since_epoch()).count();
    return static_cast<std::uint64_t>(s / stepSec);
}

std::string totpNow(
    const std::vector<std::uint8_t>& key,
    int digits, int stepSec)
{
    return hotp(key, nowStep(stepSec), digits);
}

bool totpVerify(
    const std::vector<std::uint8_t>& key,
    const std::string& code,
    int digits, int stepSec, int window)
{
    auto base = nowStep(stepSec);
    for (int i = -window; i <= window; ++i) {
        auto c = hotp(
            key, base + static_cast<std::uint64_t>(i),
            digits);
        if (c == code) return true;
    }
    return false;
}

} // namespace services::auth::totp
