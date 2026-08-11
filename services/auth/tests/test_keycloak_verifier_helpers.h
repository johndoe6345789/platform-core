#pragma once
/**
 * @file test_keycloak_verifier_helpers.h
 * @brief In-test RSA keypair generation helper.
 */

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include <string>

namespace test_kc
{

struct KeyPair
{
    std::string priv;
    std::string pub;
};

inline KeyPair makeRsa()
{
    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_CTX* ctx =
        EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    EVP_PKEY_keygen_init(ctx);
    EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048);
    EVP_PKEY_keygen(ctx, &pkey);
    EVP_PKEY_CTX_free(ctx);

    auto dump = [&](bool isPriv) {
        BIO* bio = BIO_new(BIO_s_mem());
        if (isPriv)
            PEM_write_bio_PrivateKey(bio, pkey, nullptr,
                nullptr, 0, nullptr, nullptr);
        else
            PEM_write_bio_PUBKEY(bio, pkey);
        BUF_MEM* m = nullptr;
        BIO_get_mem_ptr(bio, &m);
        std::string s(m->data, m->length);
        BIO_free(bio);
        return s;
    };
    KeyPair kp{dump(true), dump(false)};
    EVP_PKEY_free(pkey);
    return kp;
}

} // namespace test_kc
