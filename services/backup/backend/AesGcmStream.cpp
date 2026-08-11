/**
 * @file AesGcmStream.cpp
 * @brief OpenSSL EVP-based AES-256-GCM streaming encryptor.
 */

#include "AesGcmStream.h"

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <cstring>
#include <stdexcept>

namespace nextra::backup
{

namespace
{
void hexToBytes(const std::string& hex, unsigned char* out,
                std::size_t n)
{
    if (hex.size() != n * 2)
        throw std::runtime_error("AesGcmStream: bad key length");
    for (std::size_t i = 0; i < n; ++i)
    {
        unsigned int byte = 0;
        if (std::sscanf(hex.c_str() + i * 2, "%2x", &byte) != 1)
            throw std::runtime_error("AesGcmStream: bad hex");
        out[i] = static_cast<unsigned char>(byte);
    }
}
}  // namespace

AesGcmStream::AesGcmStream(const std::string& hexKey)
{
    unsigned char key[32];
    hexToBytes(hexKey, key, 32);
    if (RAND_bytes(iv_, 12) != 1)
        throw std::runtime_error("AesGcmStream: RAND_bytes");

    ctx_ = EVP_CIPHER_CTX_new();
    if (!ctx_)
        throw std::runtime_error("AesGcmStream: EVP_CIPHER_CTX_new");

    if (EVP_EncryptInit_ex(ctx_, EVP_aes_256_gcm(), nullptr,
                           key, iv_) != 1)
        throw std::runtime_error("AesGcmStream: EncryptInit");

    out_.insert(out_.end(), iv_, iv_ + 12);
}

AesGcmStream::~AesGcmStream()
{
    if (ctx_) EVP_CIPHER_CTX_free(ctx_);
}

void AesGcmStream::update(const unsigned char* pt, std::size_t n)
{
    std::vector<unsigned char> ct(n + 16);
    int outLen = 0;
    if (EVP_EncryptUpdate(ctx_, ct.data(), &outLen, pt,
                          static_cast<int>(n)) != 1)
        throw std::runtime_error("AesGcmStream: EncryptUpdate");
    out_.insert(out_.end(), ct.begin(), ct.begin() + outLen);
}

void AesGcmStream::finish()
{
    unsigned char tail[16];
    int outLen = 0;
    if (EVP_EncryptFinal_ex(ctx_, tail, &outLen) != 1)
        throw std::runtime_error("AesGcmStream: EncryptFinal");
    out_.insert(out_.end(), tail, tail + outLen);

    unsigned char tag[16];
    if (EVP_CIPHER_CTX_ctrl(ctx_, EVP_CTRL_GCM_GET_TAG, 16, tag) != 1)
        throw std::runtime_error("AesGcmStream: get tag");
    out_.insert(out_.end(), tag, tag + 16);
}

}  // namespace nextra::backup
