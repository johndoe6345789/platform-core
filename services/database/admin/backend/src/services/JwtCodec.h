/**
 * @file JwtCodec.h
 * @brief Base64url encoding and HMAC-SHA256 signing.
 */

#pragma once

#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <string>

namespace pgadmin
{

/// @brief Base64url-encode raw bytes.
/// @param data Pointer to input bytes
/// @param len Number of bytes
/// @return Base64url-encoded string
inline std::string b64url(
    const unsigned char* data, size_t len)
{
    static const char T[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
    std::string out;
    for (size_t i = 0; i < len; i += 3) {
        unsigned n = (unsigned)data[i] << 16;
        if (i + 1 < len)
            n |= (unsigned)data[i + 1] << 8;
        if (i + 2 < len) n |= data[i + 2];
        out += T[(n >> 18) & 63];
        out += T[(n >> 12) & 63];
        out += (i + 1 < len)
                   ? T[(n >> 6) & 63] : '=';
        out += (i + 2 < len)
                   ? T[n & 63] : '=';
    }
    for (auto& c : out) {
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
    }
    while (!out.empty() && out.back() == '=')
        out.pop_back();
    return out;
}

/// @brief Base64url-encode a string.
/// @param s Input string
/// @return Base64url-encoded string
inline std::string b64url(const std::string& s)
{
    return b64url(
        reinterpret_cast<const unsigned char*>(
            s.data()),
        s.size());
}

/// @brief Sign a message with HMAC-SHA256.
/// @param msg The message to sign
/// @param key The signing key
/// @return Base64url-encoded signature
inline std::string hmacSign(
    const std::string& msg,
    const std::string& key)
{
    unsigned char out[EVP_MAX_MD_SIZE];
    unsigned int len = 0;
    HMAC(EVP_sha256(),
         key.data(),
         static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char*>(
             msg.data()),
         msg.size(), out, &len);
    return b64url(out, len);
}

} // namespace pgadmin
