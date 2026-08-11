/**
 * @file AssertionVerifier.cpp
 * @brief OpenSSL-backed signature verification.
 */

#include "AssertionVerifier.h"

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

namespace services::auth::passkeys
{

namespace
{

bool verifyEs256(
    const CoseKeyParsed& k,
    const std::vector<std::uint8_t>& msg,
    const std::vector<std::uint8_t>& sig)
{
    EC_KEY* ec =
        EC_KEY_new_by_curve_name(NID_X9_62_prime256v1);
    if (!ec) return false;
    BIGNUM* bx = BN_bin2bn(k.x.data(), k.x.size(), nullptr);
    BIGNUM* by = BN_bin2bn(k.y.data(), k.y.size(), nullptr);
    EC_KEY_set_public_key_affine_coordinates(ec, bx, by);
    BN_free(bx);
    BN_free(by);
    EVP_PKEY* pk = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(pk, ec);
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    int ok = EVP_DigestVerifyInit(
                 ctx, nullptr, EVP_sha256(), nullptr, pk) == 1 &&
             EVP_DigestVerify(
                 ctx, sig.data(), sig.size(),
                 msg.data(), msg.size()) == 1;
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pk);
    return ok;
}

bool verifyRs256(
    const CoseKeyParsed& k,
    const std::vector<std::uint8_t>& msg,
    const std::vector<std::uint8_t>& sig)
{
    RSA* rsa = RSA_new();
    BIGNUM* bn = BN_bin2bn(k.n.data(), k.n.size(), nullptr);
    BIGNUM* be = BN_bin2bn(k.e.data(), k.e.size(), nullptr);
    RSA_set0_key(rsa, bn, be, nullptr);
    EVP_PKEY* pk = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pk, rsa);
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    int ok = EVP_DigestVerifyInit(
                 ctx, nullptr, EVP_sha256(), nullptr, pk) == 1 &&
             EVP_DigestVerify(
                 ctx, sig.data(), sig.size(),
                 msg.data(), msg.size()) == 1;
    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(pk);
    return ok;
}

} // namespace

bool verifyAssertion(
    const CoseKeyParsed& key,
    const std::vector<std::uint8_t>& authData,
    const std::vector<std::uint8_t>& clientDataHash,
    const std::vector<std::uint8_t>& signature)
{
    std::vector<std::uint8_t> msg = authData;
    msg.insert(
        msg.end(),
        clientDataHash.begin(), clientDataHash.end());
    if (key.alg == -7) return verifyEs256(key, msg, signature);
    if (key.alg == -257) return verifyRs256(key, msg, signature);
    return false;
}

} // namespace services::auth::passkeys
