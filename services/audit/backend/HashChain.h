#pragma once

/**
 * @file HashChain.h
 * @brief SHA-256 tamper-evident hash chain primitives.
 *
 * Each audit row's hash is SHA256(prev_hash || canonical_json).
 * `canonical_json` is a deterministic serialisation: object keys
 * are sorted, no insignificant whitespace, numbers in their
 * shortest round-trip form.  This keeps the chain reproducible
 * regardless of how the producer serialised its JSON.
 */

#include "AuditTypes.h"

#include <nlohmann/json.hpp>

#include <string>
#include <vector>

namespace nextra::audit
{

/** @brief SHA256 digest size in bytes. */
constexpr std::size_t kHashBytes = 32;

/**
 * @brief Serialise a JSON value with sorted keys.
 * @param j The source JSON value.
 * @return Canonical UTF-8 string.
 */
std::string canonicalJson(const nlohmann::json& j);

/**
 * @brief Compute SHA256(prev || canonical(payload)) as raw bytes.
 * @param prev   Previous row hash (may be empty for chain head).
 * @param event  The event whose payload is hashed.
 * @return 32-byte digest.
 */
std::vector<unsigned char> computeHash(
    const std::vector<unsigned char>& prev,
    const AuditEvent& event);

/**
 * @brief Hex-encode a byte buffer (lowercase).
 * @param bytes Raw bytes.
 * @return Lowercase hex string, twice the input length.
 */
std::string toHex(const std::vector<unsigned char>& bytes);

/**
 * @brief Decode a hex string back to bytes.  Empty string → empty.
 * @param hex Lowercase hex input.
 * @return Raw byte buffer.
 */
std::vector<unsigned char> fromHex(const std::string& hex);

}  // namespace nextra::audit
