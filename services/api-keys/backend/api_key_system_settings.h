#pragma once
/**
 * @file api_key_system_settings.h
 * @brief ResolvedKey value object for ApiKeyService.
 */

#include <string>

namespace services
{

/**
 * @brief Resolved API key and model for a provider.
 */
struct ResolvedKey {
    std::string apiKey;
    std::string model;
};

} // namespace services
