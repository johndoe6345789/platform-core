#pragma once
/**
 * @file ApiKeyFallback.h
 * @brief Fallback resolution helpers for API keys.
 *
 * Provides system-settings and env-var lookups used
 * by the ApiKeyService resolve chain.
 */

#include "ai-chat/backend/AiTypes.h"

#include <string>

namespace services
{

/**
 * @brief Look up API key from env variable.
 * @param prov Provider name ("openai"/"claude").
 * @param onOk Callback with resolved key JSON.
 */
void fallbackEnv(
    const std::string& prov,
    Callback onOk);

/**
 * @brief Look up API key from system_settings,
 *        falling back to env if not found.
 * @param prov Provider name ("openai"/"claude").
 * @param onOk Callback with resolved key JSON.
 */
void fallbackSystem(
    const std::string& prov,
    Callback onOk);

} // namespace services
