#pragma once
/**
 * @file UserLookupService.h
 * @brief Single-user lookup operations and shared DB helpers.
 *
 * Provides fetch-by-id and fetch-by-email queries.  Also
 * exposes the `db()` accessor and `rowToJson()` helper that
 * other user-domain services rely on.
 */

#include "users/backend/user_service_types.h"

#include <drogon/orm/Row.h>

#include <string>

namespace services
{

/**
 * @class UserLookupService
 * @brief Fetch individual user records from the database.
 */
class UserLookupService
{
  public:
    UserLookupService() = default;
    ~UserLookupService() = default;

    /**
     * @brief Fetch a user by primary key.
     *
     * @param id        User UUID.
     * @param onSuccess Callback with user JSON or `null`.
     * @param onError   Callback on DB error.
     */
    void getUserById(const std::string& id, Callback onSuccess,
                     ErrCallback onError);

    /**
     * @brief Fetch a user by email address.
     *
     * @param email     Registered email.
     * @param onSuccess Callback with user JSON or `null`.
     * @param onError   Callback on DB error.
     */
    void getUserByEmail(const std::string& email, Callback onSuccess,
                        ErrCallback onError);

    /**
     * @brief Map a result row to a public-facing user JSON.
     *
     * Excludes sensitive fields such as `password_hash`.
     *
     * @param row  A single result row from the `users` table.
     * @return     JSON object with camelCase field names.
     */
    [[nodiscard]] static auto rowToJson(const drogon::orm::Row& row) -> json;

  private:
    /// Convenience DB accessor.
    [[nodiscard]] static auto db() -> DbClientPtr;
};

} // namespace services
