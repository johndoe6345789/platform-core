/**
 * @file HashChain.cpp
 * @brief SHA-256 hash chain primitives — OpenSSL backed.
 */

#include "audit/backend/HashChain.h"

#include <openssl/evp.h>
#include <openssl/sha.h>

#include <cstdio>
#include <stdexcept>

namespace nextra::audit
{

std::string canonicalJson(const nlohmann::json& j)
{
    // nlohmann::json::dump with sort_keys = true produces a
    // deterministic form; pass indent = -1 and ensure_ascii = false.
    return j.dump(-1, ' ', false,
                  nlohmann::json::error_handler_t::replace);
}

std::vector<unsigned char> computeHash(
    const std::vector<unsigned char>& prev,
    const AuditEvent& event)
{
    nlohmann::json env = {
        {"tenant_id",   event.tenantId},
        {"action",      event.action},
        {"created_at",  event.createdAt},
        {"payload",     event.payload},
    };
    if (event.actorId)    env["actor_id"]    = *event.actorId;
    if (event.targetType) env["target_type"] = *event.targetType;
    if (event.targetId)   env["target_id"]   = *event.targetId;

    // Sorted dump for determinism.
    std::string canonical = env.dump();
    // nlohmann sorts by default for object; keep explicit for safety.

    std::vector<unsigned char> out(kHashBytes, 0);
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");
    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, prev.data(), prev.size()) != 1 ||
        EVP_DigestUpdate(ctx, canonical.data(),
                         canonical.size()) != 1 ||
        EVP_DigestFinal_ex(ctx, out.data(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("SHA256 digest failed");
    }
    EVP_MD_CTX_free(ctx);
    return out;
}

std::string toHex(const std::vector<unsigned char>& bytes)
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

std::vector<unsigned char> fromHex(const std::string& hex)
{
    std::vector<unsigned char> out;
    if (hex.size() % 2 != 0) return out;
    out.reserve(hex.size() / 2);
    auto hv = [](char c) -> int {
        if (c >= '0' && c <= '9') return c - '0';
        if (c >= 'a' && c <= 'f') return c - 'a' + 10;
        if (c >= 'A' && c <= 'F') return c - 'A' + 10;
        return -1;
    };
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        int hi = hv(hex[i]);
        int lo = hv(hex[i + 1]);
        if (hi < 0 || lo < 0) return {};
        out.push_back(
            static_cast<unsigned char>((hi << 4) | lo));
    }
    return out;
}

}  // namespace nextra::audit
