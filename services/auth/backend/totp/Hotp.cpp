/**
 * @file Hotp.cpp
 * @brief HMAC-SHA1 HOTP implementation via OpenSSL.
 */

#include "Hotp.h"

#include <openssl/hmac.h>

#include <array>
#include <string>

namespace services::auth::totp
{

std::string hotp(
    const std::vector<std::uint8_t>& key,
    std::uint64_t counter,
    int digits)
{
    std::array<std::uint8_t, 8> ctrBuf{};
    for (int i = 7; i >= 0; --i) {
        ctrBuf[i] = static_cast<std::uint8_t>(counter & 0xff);
        counter >>= 8;
    }
    unsigned int len = 0;
    std::uint8_t mac[EVP_MAX_MD_SIZE];
    HMAC(EVP_sha1(), key.data(),
         static_cast<int>(key.size()),
         ctrBuf.data(), ctrBuf.size(), mac, &len);
    int off = mac[len - 1] & 0x0f;
    std::uint32_t bin =
        ((mac[off] & 0x7f) << 24) |
        ((mac[off + 1] & 0xff) << 16) |
        ((mac[off + 2] & 0xff) << 8) |
        (mac[off + 3] & 0xff);
    std::uint32_t mod = 1;
    for (int i = 0; i < digits; ++i) mod *= 10;
    auto val = bin % mod;
    std::string out = std::to_string(val);
    while (static_cast<int>(out.size()) < digits)
        out.insert(out.begin(), '0');
    return out;
}

} // namespace services::auth::totp
