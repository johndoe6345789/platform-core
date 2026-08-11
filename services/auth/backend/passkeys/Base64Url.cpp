/**
 * @file Base64Url.cpp
 * @brief Base64url codec implementation.
 */

#include "Base64Url.h"

namespace services::auth::passkeys
{

static const char* ALPHA =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghij"
    "klmnopqrstuvwxyz0123456789-_";

std::string b64urlEncode(const std::vector<std::uint8_t>& b)
{
    std::string out;
    int v = 0, bits = 0;
    for (auto c : b) {
        v = (v << 8) | c;
        bits += 8;
        while (bits >= 6) {
            bits -= 6;
            out.push_back(ALPHA[(v >> bits) & 0x3f]);
        }
    }
    if (bits > 0)
        out.push_back(ALPHA[(v << (6 - bits)) & 0x3f]);
    return out;
}

std::vector<std::uint8_t> b64urlDecode(const std::string& s)
{
    int t[256];
    for (int i = 0; i < 256; ++i) t[i] = -1;
    for (int i = 0; i < 64; ++i)
        t[(unsigned)ALPHA[i]] = i;
    std::vector<std::uint8_t> out;
    int v = 0, bits = 0;
    for (char c : s) {
        int d = t[(unsigned char)c];
        if (d < 0) continue;
        v = (v << 6) | d;
        bits += 6;
        if (bits >= 8) {
            bits -= 8;
            out.push_back((v >> bits) & 0xff);
        }
    }
    return out;
}

} // namespace services::auth::passkeys
