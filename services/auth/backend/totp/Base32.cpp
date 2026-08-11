/**
 * @file Base32.cpp
 * @brief Base32 encode/decode implementation.
 */

#include "Base32.h"

namespace services::auth::totp
{

static const char* B32 =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";

std::string base32Encode(
    const std::vector<std::uint8_t>& data)
{
    std::string out;
    int v = 0, bits = 0;
    for (auto c : data) {
        v = (v << 8) | c;
        bits += 8;
        while (bits >= 5) {
            bits -= 5;
            out.push_back(B32[(v >> bits) & 0x1f]);
        }
    }
    if (bits > 0)
        out.push_back(B32[(v << (5 - bits)) & 0x1f]);
    return out;
}

std::vector<std::uint8_t> base32Decode(
    const std::string& text)
{
    int t[256];
    for (int i = 0; i < 256; ++i) t[i] = -1;
    for (int i = 0; i < 32; ++i) {
        t[(unsigned char)B32[i]] = i;
        char lc =
            (char)(B32[i] >= 'A' && B32[i] <= 'Z'
                   ? B32[i] + 32 : B32[i]);
        t[(unsigned char)lc] = i;
    }
    std::vector<std::uint8_t> out;
    int v = 0, bits = 0;
    for (char c : text) {
        if (c == '=' || c == ' ') continue;
        int d = t[(unsigned char)c];
        if (d < 0) continue;
        v = (v << 5) | d;
        bits += 5;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((v >> bits) & 0xff);
        }
    }
    return out;
}

} // namespace services::auth::totp
