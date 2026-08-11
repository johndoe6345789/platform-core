#pragma once

/**
 * @file AesGcmStream.h
 * @brief Streaming AES-256-GCM encryptor built on OpenSSL EVP.
 *
 * The ciphertext frame format is:
 *   [ 12-byte IV ][ ciphertext... ][ 16-byte GCM tag ]
 * which matches what the restore path expects from S3.  A
 * single instance encrypts one dump end-to-end; create a
 * new one per backup run.
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct evp_cipher_ctx_st;

namespace nextra::backup
{

/**
 * @brief Buffered AES-256-GCM encryptor.
 *
 * Call @ref update for every plaintext chunk, then @ref finish
 * to flush state and append the authentication tag.
 */
class AesGcmStream
{
public:
    /**
     * @brief Construct with a 32-byte hex key.
     * @param hexKey 64 hex chars (256 bits).
     * @throws std::runtime_error if the key is malformed.
     */
    explicit AesGcmStream(const std::string& hexKey);
    ~AesGcmStream();

    AesGcmStream(const AesGcmStream&)            = delete;
    AesGcmStream& operator=(const AesGcmStream&) = delete;

    /// Feed plaintext and append ciphertext to the output buffer.
    void update(const unsigned char* pt, std::size_t n);

    /// Flush final block and append the 16-byte GCM tag.
    void finish();

    /// Access the accumulated output (IV ++ ct ++ tag).
    const std::vector<unsigned char>& output() const
    {
        return out_;
    }

private:
    evp_cipher_ctx_st*         ctx_{nullptr};
    std::vector<unsigned char> out_;
    unsigned char              iv_[12]{};
};

}  // namespace nextra::backup
