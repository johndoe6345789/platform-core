/**
 * @file CoseKey.cpp
 * @brief COSE_Key parser implementation.
 */

#include "CoseKey.h"

#include <stdexcept>

namespace services::auth::passkeys
{

namespace
{

std::int64_t asInt(const cbor::ValuePtr& v)
{
    if (!v) return 0;
    if (auto p = std::get_if<std::int64_t>(&v->data))
        return *p;
    return 0;
}

cbor::Bytes asBytes(const cbor::ValuePtr& v)
{
    if (!v) return {};
    if (auto p = std::get_if<cbor::Bytes>(&v->data))
        return *p;
    return {};
}

} // namespace

CoseKeyParsed parseCoseKey(const cbor::Map& map)
{
    CoseKeyParsed out;
    out.kty = static_cast<int>(asInt(cbor::mapGetInt(map, 1)));
    out.alg = static_cast<int>(asInt(cbor::mapGetInt(map, 3)));
    if (out.kty == 2) {
        // EC2
        out.crv = static_cast<int>(
            asInt(cbor::mapGetInt(map, -1)));
        out.x = asBytes(cbor::mapGetInt(map, -2));
        out.y = asBytes(cbor::mapGetInt(map, -3));
        if (out.x.empty() || out.y.empty())
            throw std::runtime_error("cose ec2 missing xy");
    } else if (out.kty == 3) {
        // RSA
        out.n = asBytes(cbor::mapGetInt(map, -1));
        out.e = asBytes(cbor::mapGetInt(map, -2));
        if (out.n.empty() || out.e.empty())
            throw std::runtime_error("cose rsa missing ne");
    } else {
        throw std::runtime_error("cose kty unsupported");
    }
    return out;
}

} // namespace services::auth::passkeys
