/**
 * @file UserService.cpp
 * @brief Facade implementation — delegates to sub-services.
 */

#include "users/backend/UserService.h"

namespace services
{

void UserService::getUserById(const std::string& id, Callback onSuccess,
                              ErrCallback onError)
{
    lookup_.getUserById(id, onSuccess, onError);
}

void UserService::getUserByEmail(const std::string& email, Callback onSuccess,
                                 ErrCallback onError)
{
    lookup_.getUserByEmail(email, onSuccess, onError);
}

void UserService::updateUser(const std::string& id, const json& fields,
                             Callback onSuccess, ErrCallback onError)
{
    profile_.updateUser(id, fields, onSuccess, onError);
}

void UserService::listUsers(std::int32_t page, std::int32_t perPage,
                            Callback onSuccess, ErrCallback onError)
{
    search_.listUsers(page, perPage, onSuccess, onError);
}

void UserService::getUserBadges(const std::string& userId, Callback onSuccess,
                                ErrCallback onError)
{
    badges_.getUserBadges(userId, onSuccess, onError);
}

void UserService::getUserStats(const std::string& userId, Callback onSuccess,
                               ErrCallback onError)
{
    stats_.getUserStats(userId, onSuccess, onError);
}

} // namespace services
