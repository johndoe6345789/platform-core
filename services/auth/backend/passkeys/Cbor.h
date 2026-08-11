#pragma once
/**
 * @file Cbor.h
 * @brief Minimal CBOR decoder for WebAuthn attestationObject.
 *
 * Supports only the subset needed by the spec: unsigned ints,
 * negative ints, byte strings, text strings, arrays, and maps.
 * Returns a tagged variant tree.  Does NOT validate canonical
 * encoding — callers must treat input as untrusted.
 */

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace services::auth::passkeys::cbor
{

struct Value;
using ValuePtr = std::shared_ptr<Value>;
using Bytes = std::vector<std::uint8_t>;
using Array = std::vector<ValuePtr>;
using Map = std::vector<std::pair<ValuePtr, ValuePtr>>;

struct Value
{
    std::variant<std::int64_t, Bytes, std::string,
                 Array, Map>
        data;
};

/** @brief Decode a CBOR blob starting at offset; advances it. */
ValuePtr decode(const Bytes& buf, std::size_t& off);

/** @brief Look up an integer key in a map; nullptr if absent. */
ValuePtr mapGetInt(const Map& m, std::int64_t key);

/** @brief Look up a string key in a map. */
ValuePtr mapGetStr(const Map& m, const std::string& key);

} // namespace services::auth::passkeys::cbor
