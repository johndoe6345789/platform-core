#pragma once
/**
 * @file UserSearchService.h
 * @brief Paginated user listing operations.
 *
 * Provides a paginated query over the `users` table with
 * total count metadata returned alongside the data array.
 */

#include "users/backend/user_service_types.h"

#include <cstdint>
#include <string>

namespace services
{

/**
 * @class UserSearchService
 * @brief Paginated listing of user records.
 */
class UserSearchService
{
  public:
    UserSearchService() = default;
    ~UserSearchService() = default;

    /**
     * @brief Paginated list of all users.
     *
     * @param page      1-based page number (clamped to >= 1).
     * @param perPage   Items per page (clamped to 1–100).
     * @param onSuccess Callback with paginated result JSON.
     * @param onError   Callback on failure.
     */
    void listUsers(std::int32_t page, std::int32_t perPage, Callback onSuccess,
                   ErrCallback onError);

  private:
    /// Convenience DB accessor.
    [[nodiscard]] static auto db() -> DbClientPtr;
};

} // namespace services
