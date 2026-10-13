#pragma once
/**
 * @file PublicProfileController.h
 * @brief Public read-only founder profile endpoint.
 *
 * GET /api/users/profile/{username}
 *   No auth required. Returns public profile fields.
 */

#include <drogon/HttpController.h>
#include <string>

namespace controllers
{

/**
 * @class PublicProfileController
 * @brief Serves the public founder profile card data.
 */
class PublicProfileController
    : public drogon::HttpController<
        PublicProfileController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        PublicProfileController::getPublicProfile,
        "/api/users/profile/{username}",
        drogon::Get);
    METHOD_LIST_END

    /**
     * @brief Get a founder's public profile by username.
     *
     * Returns: username, displayName, startupName,
     * startupType, stage, bio, is_mentor, joined_at,
     * badge_count, current_streak.
     *
     * @param req      HTTP request (no auth needed).
     * @param cb       Response callback.
     * @param username URL path segment.
     */
    void getPublicProfile(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& username);
};

} // namespace controllers
