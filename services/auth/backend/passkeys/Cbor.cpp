/**
 * @file Cbor.cpp
 * @brief Minimal CBOR decoder implementation.
 */

#include "Cbor.h"
#include "CborRead.h"

#include <stdexcept>

namespace services::auth::passkeys::cbor
{

ValuePtr decode(const Bytes& buf, std::size_t& off)
{
    if (off >= buf.size())
        throw std::runtime_error("cbor eof");
    std::uint8_t ib = buf[off++];
    std::uint8_t major = ib >> 5;
    std::uint8_t info = ib & 0x1f;
    std::uint64_t arg = readArg(buf, off, info);
    auto out = std::make_shared<Value>();
    if (major == 0) {
        out->data = std::int64_t(arg);
    } else if (major == 1) {
        out->data = -1 - std::int64_t(arg);
    } else if (major == 2) {
        if (off + arg > buf.size())
            throw std::runtime_error("cbor eof");
        Bytes bs(buf.begin() + off,
                 buf.begin() + off + arg);
        off += arg;
        out->data = std::move(bs);
    } else if (major == 3) {
        if (off + arg > buf.size())
            throw std::runtime_error("cbor eof");
        std::string s(
            reinterpret_cast<const char*>(&buf[off]), arg);
        off += arg;
        out->data = std::move(s);
    } else if (major == 4) {
        Array a;
        a.reserve(arg);
        for (std::uint64_t i = 0; i < arg; ++i)
            a.push_back(decode(buf, off));
        out->data = std::move(a);
    } else if (major == 5) {
        Map m;
        m.reserve(arg);
        for (std::uint64_t i = 0; i < arg; ++i) {
            auto k = decode(buf, off);
            auto v = decode(buf, off);
            m.emplace_back(k, v);
        }
        out->data = std::move(m);
    } else {
        throw std::runtime_error("cbor major unsupported");
    }
    return out;
}

ValuePtr mapGetInt(const Map& m, std::int64_t key)
{
    for (const auto& [k, v] : m) {
        if (auto p = std::get_if<std::int64_t>(&k->data))
            if (*p == key) return v;
    }
    return nullptr;
}

ValuePtr mapGetStr(const Map& m, const std::string& key)
{
    for (const auto& [k, v] : m) {
        if (auto p = std::get_if<std::string>(&k->data))
            if (*p == key) return v;
    }
    return nullptr;
}

} // namespace services::auth::passkeys::cbor
