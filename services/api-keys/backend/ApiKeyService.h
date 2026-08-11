#pragma once
/**
 * @file ApiKeyService.h
 * @brief Resolves AI provider API keys and models
 *        using user key -> system key -> env chain.
 */

#include "ai-chat/backend/AiTypes.h"
#include "api-keys/backend/api_key_system_settings.h"

#include <string>

namespace services
{

/**
 * @class ApiKeyService
 * @brief Looks up API keys from user_api_keys,
 *        system_settings, then environment variables.
 */
class ApiKeyService
{
  public:
    /** @brief Resolve key for a provider. */
    static void resolve(
        const std::string& userId,
        AiProvider provider,
        Callback onOk, ErrCallback onErr);

    /** @brief Save or update a user's API key. */
    static void setUserKey(
        const std::string& userId,
        const std::string& provider,
        const std::string& apiKey,
        const std::string& model,
        Callback onOk, ErrCallback onErr);

    /** @brief Get a user's stored key (masked). */
    static void getUserKey(
        const std::string& userId,
        const std::string& provider,
        Callback onOk, ErrCallback onErr);

    /** @brief Delete a user's stored key. */
    static void deleteUserKey(
        const std::string& userId,
        const std::string& provider,
        Callback onOk, ErrCallback onErr);

    /** @brief Get a system setting value. */
    static void getSystemSetting(
        const std::string& key,
        Callback onOk, ErrCallback onErr);

    /** @brief Set a system setting value. */
    static void setSystemSetting(
        const std::string& key,
        const std::string& value,
        Callback onOk, ErrCallback onErr);

    /** @brief Get all user keys (masked). */
    static void getAllUserKeys(
        const std::string& userId,
        Callback onOk, ErrCallback onErr);

  private:
    [[nodiscard]] static auto db() -> DbClientPtr;

    /** @brief Mask an API key for display. */
    [[nodiscard]] static auto mask(
        const std::string& key) -> std::string;
};

} // namespace services
