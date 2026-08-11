/**
 * @file AttestationStatement.cpp
 * @brief Statement verification for "none" and packed attestations.
 */

#include "AttestationStatement.h"
#include "AssertionVerifier.h"

#include <openssl/evp.h>
#include <openssl/x509.h>
#include <stdexcept>

namespace services::auth::passkeys
{

namespace
{

auto intValue(const cbor::ValuePtr& v) -> std::int64_t
{
    auto p = v ? std::get_if<std::int64_t>(&v->data) : nullptr;
    if (!p) throw std::runtime_error("att stmt int");
    return *p;
}

auto bytesValue(const cbor::ValuePtr& v) -> cbor::Bytes
{
    auto p = v ? std::get_if<cbor::Bytes>(&v->data) : nullptr;
    if (!p) throw std::runtime_error("att stmt bytes");
    return *p;
}

auto firstCert(const cbor::ValuePtr& v) -> cbor::Bytes
{
    auto arr = v ? std::get_if<cbor::Array>(&v->data) : nullptr;
    if (!arr || arr->empty()) return {};
    return bytesValue((*arr)[0]);
}

auto verifyPackedCert(const cbor::Bytes& certDer,
                      const cbor::Bytes& authData,
                      const cbor::Bytes& hash,
                      const cbor::Bytes& sig,
                      int alg) -> bool
{
    if (alg != -7 && alg != -257) return false;
    auto msg = authData;
    msg.insert(msg.end(), hash.begin(), hash.end());
    const unsigned char* ptr = certDer.data();
    X509* cert = d2i_X509(nullptr, &ptr, certDer.size());
    if (!cert) return false;
    EVP_PKEY* key = X509_get_pubkey(cert);
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    int ok = key && ctx &&
             EVP_DigestVerifyInit(
                 ctx, nullptr, EVP_sha256(), nullptr, key) == 1 &&
             EVP_DigestVerify(
                 ctx, sig.data(), sig.size(),
                 msg.data(), msg.size()) == 1;
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(key);
    X509_free(cert);
    return ok;
}

} // namespace

void verifyAttestationStatement(
    const std::string& fmt,
    const cbor::Map& stmt,
    const CoseKeyParsed& key,
    const std::vector<std::uint8_t>& authData,
    const std::vector<std::uint8_t>& clientDataHash)
{
    if (fmt == "none") return;
    if (fmt != "packed")
        throw std::runtime_error("att fmt unsupported");
    auto alg = static_cast<int>(intValue(cbor::mapGetStr(stmt, "alg")));
    auto sig = bytesValue(cbor::mapGetStr(stmt, "sig"));
    auto cert = firstCert(cbor::mapGetStr(stmt, "x5c"));
    if (!cert.empty()) {
        if (!verifyPackedCert(
                cert, authData, clientDataHash, sig, alg))
            throw std::runtime_error("packed sig invalid");
        return;
    }
    if (alg != key.alg)
        throw std::runtime_error("packed alg mismatch");
    if (!verifyAssertion(key, authData, clientDataHash, sig))
        throw std::runtime_error("packed sig invalid");
}

} // namespace services::auth::passkeys
