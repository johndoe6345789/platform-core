/**
 * @file jwks_client_helpers.cpp
 * @brief Implementation of JWK->PEM and URL split helpers.
 */

#include "jwks_client_helpers.h"

#define JWT_DISABLE_PICOJSON
#include <jwt-cpp/traits/nlohmann-json/defaults.h>

#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <memory>

namespace services::auth::keycloak::detail
{

static std::string b64uDecode(const std::string& s)
{
    using namespace jwt;
    return base::decode<alphabet::base64url>(
        base::pad<alphabet::base64url>(s));
}

std::string rsaPemFromJwk(const std::string& nB64u,
                          const std::string& eB64u)
{
    auto nRaw = b64uDecode(nB64u);
    auto eRaw = b64uDecode(eB64u);
    BIGNUM* n = BN_bin2bn(
        reinterpret_cast<const unsigned char*>(nRaw.data()),
        static_cast<int>(nRaw.size()), nullptr);
    BIGNUM* e = BN_bin2bn(
        reinterpret_cast<const unsigned char*>(eRaw.data()),
        static_cast<int>(eRaw.size()), nullptr);
    if (!n || !e) {
        BN_free(n); BN_free(e);
        return {};
    }
    RSA* rsa = RSA_new();
    if (!rsa || RSA_set0_key(rsa, n, e, nullptr) != 1) {
        RSA_free(rsa); BN_free(n); BN_free(e);
        return {};
    }
    EVP_PKEY* pkey = EVP_PKEY_new();
    if (EVP_PKEY_assign_RSA(pkey, rsa) != 1) {
        EVP_PKEY_free(pkey); RSA_free(rsa);
        return {};
    }
    std::unique_ptr<BIO, decltype(&BIO_free)> bio{
        BIO_new(BIO_s_mem()), &BIO_free};
    if (!bio || PEM_write_bio_PUBKEY(bio.get(), pkey) != 1) {
        EVP_PKEY_free(pkey);
        return {};
    }
    BUF_MEM* mem = nullptr;
    BIO_get_mem_ptr(bio.get(), &mem);
    std::string pem(mem->data, mem->length);
    EVP_PKEY_free(pkey);
    return pem;
}

std::pair<std::string, std::string>
splitUrl(const std::string& url)
{
    auto schemeEnd = url.find("://");
    if (schemeEnd == std::string::npos) return {url, "/"};
    auto pathStart = url.find('/', schemeEnd + 3);
    if (pathStart == std::string::npos)
        return {url, "/"};
    return {url.substr(0, pathStart),
            url.substr(pathStart)};
}

} // namespace services::auth::keycloak::detail
