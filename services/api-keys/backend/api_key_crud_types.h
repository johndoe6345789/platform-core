#pragma once
/**
 * @file api_key_crud_types.h
 * @brief CRUD method declarations extracted from
 *        ApiKeyService for user key management.
 *
 * These declarations are part of ApiKeyService but
 * separated for file-size limits.
 */

#include "ai-chat/backend/AiTypes.h"

#include <string>

namespace services
{

/**
 * @brief Masks an API key for safe display.
 * @param key  The full API key.
 * @return Masked string showing first/last 4 chars.
 */
[[nodiscard]] inline auto maskApiKey(
    const std::string& key) -> std::string
{
    if (key.size() <= 8) return "****";
    return key.substr(0, 4) + "..."
           + key.substr(key.size() - 4);
}

} // namespace services
