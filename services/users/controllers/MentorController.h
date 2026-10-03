#pragma once
/**
 * @file MentorController.h
 * @brief PATCH /api/users/me/mentor — toggle mentor opt-in.
 */

#include <drogon/HttpController.h>

namespace controllers
{

/**
 * @brief Allows authenticated users to opt in/out as mentor.
 */
class MentorController
    : public drogon::HttpController<MentorController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        MentorController::toggleMentor,
        "/api/users/me/mentor",
        drogon::Patch,
        "filters::JwtAuthFilter");
    METHOD_LIST_END

    /**
     * @brief Toggle is_mentor for the authenticated user.
     * @param req HTTP request (expects JSON body
     *            `{"is_mentor": true|false}`).
     * @param cb  Response callback.
     */
    void toggleMentor(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace controllers
