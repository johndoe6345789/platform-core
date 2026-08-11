/**
 * @file test_preferences_merge.cpp
 * @brief Unit tests for the preferences
 *        read-merge-write logic.
 *
 * Validates that partial updates preserve
 * existing values instead of clobbering them.
 */

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief Simulates the merge logic from
 *        PreferencesController::update.
 *
 * @param current  Current prefs in DB.
 * @param incoming Partial update payload.
 * @return Merged preferences.
 */
static json mergePrefs(
    const json& current,
    const json& incoming)
{
    auto tm = incoming.value(
        "themeMode",
        current.value("themeMode", "system"));
    auto lc = incoming.value(
        "locale",
        current.value("locale", "en"));
    return {{"themeMode", tm}, {"locale", lc}};
}

TEST(PreferencesMerge, PartialThemePreservesLocale)
{
    json cur = {
        {"themeMode", "light"},
        {"locale", "ja"},
    };
    json incoming = {{"themeMode", "dark"}};
    auto result = mergePrefs(cur, incoming);

    EXPECT_EQ(result["themeMode"], "dark");
    EXPECT_EQ(result["locale"], "ja");
}

TEST(PreferencesMerge, PartialLocalePreservesTheme)
{
    json cur = {
        {"themeMode", "dark"},
        {"locale", "en"},
    };
    json incoming = {{"locale", "fr"}};
    auto result = mergePrefs(cur, incoming);

    EXPECT_EQ(result["themeMode"], "dark");
    EXPECT_EQ(result["locale"], "fr");
}

TEST(PreferencesMerge, EmptyIncomingKeepsCurrent)
{
    json cur = {
        {"themeMode", "system"},
        {"locale", "de"},
    };
    json incoming = json::object();
    auto result = mergePrefs(cur, incoming);

    EXPECT_EQ(result["themeMode"], "system");
    EXPECT_EQ(result["locale"], "de");
}

TEST(PreferencesMerge, FullUpdateOverwritesBoth)
{
    json cur = {
        {"themeMode", "light"},
        {"locale", "en"},
    };
    json incoming = {
        {"themeMode", "dark"},
        {"locale", "zh"},
    };
    auto result = mergePrefs(cur, incoming);

    EXPECT_EQ(result["themeMode"], "dark");
    EXPECT_EQ(result["locale"], "zh");
}

TEST(PreferencesMerge, EmptyCurrentUsesDefaults)
{
    json cur = json::object();
    json incoming = json::object();
    auto result = mergePrefs(cur, incoming);

    EXPECT_EQ(result["themeMode"], "system");
    EXPECT_EQ(result["locale"], "en");
}
