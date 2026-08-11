/**
 * @file GoogleProvider.cpp
 * @brief Google userinfo -> UserProfile mapping.
 */

#include "GoogleProvider.h"

namespace services::auth::oauth::google
{

UserProfile mapProfile(const nlohmann::json& j)
{
    UserProfile p;
    if (j.contains("sub"))
        p.subject = j["sub"].get<std::string>();
    if (j.contains("email"))
        p.email = j["email"].get<std::string>();
    if (j.contains("name"))
        p.displayName = j["name"].get<std::string>();
    if (j.contains("picture"))
        p.avatarUrl = j["picture"].get<std::string>();
    return p;
}

} // namespace services::auth::oauth::google
