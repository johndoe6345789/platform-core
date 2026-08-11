#pragma once

#include <drogon/orm/DbClient.h>

#include <filesystem>

namespace commands::seed
{

/**
 * @brief Upsert users and streaks from a seed file.
 * @param path Seed file path.
 * @param db Default Drogon database client.
 */
void loadUsers(const std::filesystem::path& path,
               const drogon::orm::DbClientPtr& db);

/**
 * @brief Upsert badges and badge criteria from a seed file.
 * @param path Seed file path.
 * @param db Default Drogon database client.
 */
void loadBadges(const std::filesystem::path& path,
                const drogon::orm::DbClientPtr& db);

/**
 * @brief Upsert feature toggles from a seed file.
 * @param path Seed file path.
 * @param db Default Drogon database client.
 */
void loadFeatureToggles(const std::filesystem::path& path,
                        const drogon::orm::DbClientPtr& db);

/**
 * @brief Upsert notifications and notification links from a seed file.
 * @param path Seed file path.
 * @param db Default Drogon database client.
 */
void loadNotifications(const std::filesystem::path& path,
                       const drogon::orm::DbClientPtr& db);

} // namespace commands::seed
