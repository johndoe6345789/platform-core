/**
 * @file seed_notifications.cpp
 * @brief Notification + link seed loading.
 */

#include "seed_handlers.h"
#include "seed_io.h"

#include <stdexcept>
#include <string>

namespace commands::seed
{

void loadNotifications(
    const std::filesystem::path& path,
    const drogon::orm::DbClientPtr& db)
{
    for (const auto& note : readSeedArray(path)) {
        auto userKey = note.at("user").get<std::string>();
        auto users = db->execSqlSync(
            "SELECT id FROM users WHERE username = $1 OR email = $1"
            " LIMIT 1",
            userKey);
        if (users.empty())
            throw std::runtime_error("Notification seed user missing");
        auto userId = users[0]["id"].as<std::string>();
        auto existing = db->execSqlSync(
            "SELECT id FROM notifications"
            " WHERE user_id = $1 AND title = $2 AND body = $3 AND type = $4"
            " LIMIT 1",
            userId, note.at("title").get<std::string>(),
            note.value("body", std::string{}),
            note.value("type", "info"));
        std::string id;
        if (existing.empty()) {
            auto created = db->execSqlSync(
                "INSERT INTO notifications"
                " (user_id, title, body, type, is_read, created_at)"
                " VALUES ($1,$2,$3,$4,$5,NOW()) RETURNING id",
                userId, note.at("title").get<std::string>(),
                note.value("body", std::string{}),
                note.value("type", "info"),
                note.value("is_read", false));
            id = created[0]["id"].as<std::string>();
        } else {
            id = existing[0]["id"].as<std::string>();
            db->execSqlSync(
                "UPDATE notifications SET is_read = $1 WHERE id = $2",
                note.value("is_read", false), id);
        }
        db->execSqlSync(
            "DELETE FROM notification_links WHERE notification_id = $1",
            id);
        for (const auto& link : note.value("links", json::array())) {
            db->execSqlSync(
                "INSERT INTO notification_links"
                " (notification_id, rel, href) VALUES ($1,$2,$3)",
                id, link.value("rel", "related"),
                link.at("href").get<std::string>());
        }
    }
}

} // namespace commands::seed
