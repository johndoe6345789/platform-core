#pragma once
/**
 * @file registration_service.h
 * @brief New-user registration.
 */

#include "auth_service_types.h"

#include <string>

namespace services
{

/**
 * @class RegistrationService
 * @brief Validates input, hashes password, persists a user.
 */
class RegistrationService
{
  public:
    RegistrationService() = default;
    ~RegistrationService() = default;

    /**
     * @brief Register a new user account.
     * @param email       User email (must be unique).
     * @param username    Desired username (3-30 alnum).
     * @param password    Plain-text password (>=8 chars).
     * @param displayName Optional display name.
     * @param onSuccess   Callback with created user JSON.
     * @param onError     Callback on validation/DB error.
     */
    void registerUser(
        const std::string& email,
        const std::string& username,
        const std::string& password,
        const std::string& displayName,
        Callback onSuccess,
        ErrCallback onError);
};

} // namespace services
