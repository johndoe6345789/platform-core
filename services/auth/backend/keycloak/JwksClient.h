#pragma once
/**
 * @file JwksClient.h
 * @brief Fetches and caches the realm's JWKS document.
 *
 * Refreshes every 10 minutes; force-refresh on missing kid.
 * Returns RSA public keys in PEM form keyed by `kid`.
 */

#include <chrono>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

namespace services::auth::keycloak
{

/**
 * @brief Thread-safe JWKS cache for the Keycloak realm.
 */
class JwksClient
{
  public:
    /**
     * @brief Construct a JwksClient with a JWKS URL.
     * @param jwksUrl Absolute URL of realm's JWKS endpoint.
     */
    explicit JwksClient(std::string jwksUrl);

    /**
     * @brief Look up an RSA public key in PEM form by `kid`.
     * @param kid Key identifier from the JWT header.
     * @return PEM-encoded public key or std::nullopt if absent.
     */
    [[nodiscard]] std::optional<std::string>
    getPublicKeyPem(const std::string& kid);

    /// Force a refresh; primarily for tests.
    void refresh();

    /// Inject a key directly (test seam).
    void injectForTest(const std::string& kid,
                      const std::string& pem);

  private:
    std::string jwksUrl_;
    std::mutex mu_;
    std::unordered_map<std::string, std::string> keys_;
    std::chrono::steady_clock::time_point fetchedAt_{};

    bool stale_() const;
    void doFetch_();
};

} // namespace services::auth::keycloak
