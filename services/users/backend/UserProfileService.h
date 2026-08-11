#pragma once
/**
 * @file UserProfileService.h
 * @brief User profile field update operations.
 *
 * Allows callers to update the whitelisted editable fields
 * of a user record: display_name, username, avatar_url, bio.
 */

#include "users/backend/user_service_types.h"

#include <string>
#include <vector>

namespace services
{

/**
 * @class UserProfileService
 * @brief Update mutable user profile fields.
 */
class UserProfileService
{
  public:
    UserProfileService() = default;
    ~UserProfileService() = default;

    /**
     * @brief Update editable user fields.
     *
     * Supported keys in @p fields:
     * `display_name`, `username`, `avatar_url`, `bio`.
     *
     * @param id        User UUID.
     * @param fields    JSON object of key/value pairs.
     * @param onSuccess Callback with updated user JSON.
     * @param onError   Callback on failure.
     */
    void updateUser(const std::string& id, const json& fields,
                    Callback onSuccess, ErrCallback onError);

  private:
    /// Convenience DB accessor.
    [[nodiscard]] static auto db() -> DbClientPtr;

    /// Whitelist of columns that may be updated.
    static inline const std::vector<std::string> kEditableFields = {
        "display_name", "username", "avatar_url", "bio"};
};

} // namespace services
