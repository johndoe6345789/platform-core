#pragma once
/**
 * @file ProviderConfigLoader.h
 * @brief Loads OAuth ProviderConfig from constants/oauth.json
 *        plus the matching env vars for client id / secret.
 */

#include "OAuthTypes.h"

#include <optional>
#include <string>

namespace services::auth::oauth
{

/**
 * @brief Load the provider config for @p provider.
 * @param provider Canonical name: google, github, microsoft.
 * @return Populated ProviderConfig or nullopt when the
 *         provider is unknown or constants file is missing.
 */
[[nodiscard]] std::optional<ProviderConfig>
loadProviderConfig(const std::string& provider);

/**
 * @brief Return whether the provider expects
 *        `Accept: application/json` on token exchange
 *        (GitHub otherwise returns form-encoded).
 */
[[nodiscard]] bool
providerWantsJsonAccept(const std::string& provider);

/** @brief Read the portal origin fallback for redirect_uri. */
[[nodiscard]] std::string defaultPortalOrigin();

} // namespace services::auth::oauth
