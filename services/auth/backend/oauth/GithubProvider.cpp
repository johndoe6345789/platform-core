/**
 * @file GithubProvider.cpp
 * @brief GitHub /user -> UserProfile mapping.
 */

#include "GithubProvider.h"

namespace services::auth::oauth::github
{

UserProfile mapProfile(const nlohmann::json& j)
{
    UserProfile p;
    if (j.contains("id"))
        p.subject = std::to_string(j["id"].get<std::int64_t>());
    if (j.contains("email") && !j["email"].is_null())
        p.email = j["email"].get<std::string>();
    if (j.contains("name") && !j["name"].is_null())
        p.displayName = j["name"].get<std::string>();
    if (j.contains("avatar_url"))
        p.avatarUrl = j["avatar_url"].get<std::string>();
    return p;
}

} // namespace services::auth::oauth::github
