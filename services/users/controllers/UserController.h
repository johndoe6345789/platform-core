#pragma once
/**
 * @file UserController.h
 * @brief User management endpoints: list, profile, update,
 *        badges, and gamification stats.
 */

#include <drogon/HttpController.h>
#include <string>

namespace controllers
{

class UserController : public drogon::HttpController<UserController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserController::list, "/api/users", drogon::Get,
                  "filters::JwtAuthFilter");
    ADD_METHOD_TO(UserController::getProfile, "/api/users/{id}", drogon::Get);
    ADD_METHOD_TO(UserController::updateProfile, "/api/users/{id}",
                  drogon::Patch, "filters::JwtAuthFilter");
    ADD_METHOD_TO(UserController::getBadges, "/api/users/{id}/badges",
                  drogon::Get);
    ADD_METHOD_TO(UserController::getStats, "/api/users/{id}/stats",
                  drogon::Get);
    ADD_METHOD_TO(UserController::setRole, "/api/admin/users/{id}/role",
                  drogon::Patch, "filters::JwtAuthFilter");
    ADD_METHOD_TO(UserController::setActive, "/api/admin/users/{id}/active",
                  drogon::Patch, "filters::JwtAuthFilter");
    METHOD_LIST_END

    /** @brief List users with pagination (admin only). */
    void list(const drogon::HttpRequestPtr& req,
              std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    /** @brief Get a user's public profile by ID. */
    void getProfile(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                    const std::string& id);

    /** @brief Update own profile (JWT, owner only). */
    void updateProfile(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                       const std::string& id);

    /** @brief Get badges earned by a user. */
    void getBadges(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                   const std::string& id);

    /** @brief Get gamification stats for a user. */
    void getStats(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                  const std::string& id);

    /** @brief Change a user's role (admin). */
    void setRole(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& id);

    /** @brief Toggle user active status (admin). */
    void setActive(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb,
        const std::string& id);
};

} // namespace controllers
