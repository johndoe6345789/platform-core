#pragma once
/**
 * @file PreferencesService.h
 * @brief Read/write user display preferences
 *        (theme_mode, preferred_locale,
 *         ai_provider).
 */

#include "gamification/backend/gamification_types.h"

#include <string>

namespace services
{

/**
 * @class PreferencesService
 * @brief Persists per-user theme, locale, and
 *        AI provider preferences in users table.
 */
class PreferencesService
{
  public:
    /**
     * @brief Fetch preferences for a user.
     * @param userId UUID of the user.
     * @param onOk JSON with preferences.
     * @param onErr Error callback.
     */
    static void get(
        const std::string& userId,
        Callback onOk, ErrCallback onErr);

    /**
     * @brief Update preferences.
     * @param userId UUID of the user.
     * @param themeMode "light","dark","system".
     * @param locale Preferred locale code.
     * @param aiProvider "claude" or "openai".
     * @param onOk JSON echo of saved values.
     * @param onErr Error callback.
     */
    static void update(
        const std::string& userId,
        const std::string& themeMode,
        const std::string& locale,
        const std::string& aiProvider,
        Callback onOk, ErrCallback onErr);

  private:
    [[nodiscard]] static auto db() -> DbClientPtr;
};

} // namespace services
