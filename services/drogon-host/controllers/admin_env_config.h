#pragma once
/**
 * @file admin_env_config.h
 * @brief Allowlisted env variable definitions and
 *        masking logic for AdminEnvController.
 */

#include <array>
#include <string>
#include <string_view>

namespace controllers
{

/// Env vars to expose (grouped by section).
static constexpr std::array kVars = {
    std::pair{"DB_HOST", "database"},
    std::pair{"DB_PORT", "database"},
    std::pair{"DB_NAME", "database"},
    std::pair{"DB_USER", "database"},
    std::pair{"SMTP_HOST", "mail"},
    std::pair{"SMTP_PORT", "mail"},
    std::pair{"SMTP_USER", "mail"},
    std::pair{"SMTP_FROM", "mail"},
    std::pair{"ES_HOST", "search"},
    std::pair{"ES_PORT", "search"},
    std::pair{"APP_BASE_URL", "app"},
    std::pair{"ANTHROPIC_API_KEY", "ai"},
    std::pair{"OPENAI_API_KEY", "ai"},
};

/// Keys that should be masked.
static constexpr std::array kSensitive = {
    "ANTHROPIC_API_KEY",
    "OPENAI_API_KEY",
    "DB_PASSWORD",
    "JWT_SECRET",
    "SMTP_PASS",
};

/**
 * @brief Check if an env var key is sensitive.
 * @param key  The environment variable name.
 * @return True if the value should be masked.
 */
inline auto isSensitive(
    std::string_view key) -> bool
{
    for (const auto& s : kSensitive) {
        if (key == s) return true;
    }
    return false;
}

/**
 * @brief Mask a sensitive value for display.
 * @param v  The raw value.
 * @return Masked string (first/last 4 chars).
 */
inline auto maskValue(
    const std::string& v) -> std::string
{
    if (v.size() <= 8) return "****";
    return v.substr(0, 4) + "..."
           + v.substr(v.size() - 4);
}

} // namespace controllers
