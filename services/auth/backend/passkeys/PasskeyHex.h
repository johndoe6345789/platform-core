#pragma once
/**
 * @file PasskeyHex.h
 * @brief Hex helpers for passkey bytea storage.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace services::auth::passkeys
{

/** @brief Hex-encode passkey bytes for SQL decode(...,'hex'). */
inline auto toHex(const std::vector<std::uint8_t>& bytes) -> std::string
{
    static constexpr char kHex[] = "0123456789abcdef";
    std::string out;
    out.reserve(bytes.size() * 2);
    for (auto b : bytes) {
        out.push_back(kHex[(b >> 4) & 0xF]);
        out.push_back(kHex[b & 0xF]);
    }
    return out;
}

/** @brief Decode a plain hex string back to raw bytes. */
inline auto fromHex(const std::string& hex)
    -> std::vector<std::uint8_t>
{
    auto hv = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    std::vector<std::uint8_t> out;
    if (hex.size() % 2 != 0) return out;
    out.reserve(hex.size() / 2);
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        int hi = hv(hex[i]);
        int lo = hv(hex[i + 1]);
        if (hi < 0 || lo < 0) return {};
        out.push_back(static_cast<std::uint8_t>((hi << 4) | lo));
    }
    return out;
}

} // namespace services::auth::passkeys
