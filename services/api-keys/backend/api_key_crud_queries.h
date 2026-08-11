#pragma once
/**
 * @file api_key_crud_queries.h
 * @brief SQL queries for API key CRUD operations.
 */

#include <string_view>

namespace services
{

/// @brief Upsert a user API key.
constexpr std::string_view kSetUserKeySql = R"(
    INSERT INTO user_api_keys
        (user_id, provider, api_key, model)
    VALUES ($1, $2, $3, $4)
    ON CONFLICT (user_id, provider)
    DO UPDATE SET api_key = $3,
        model = $4, updated_at = NOW()
    RETURNING provider, model
)";

/// @brief Fetch a user's API key by provider.
constexpr std::string_view kGetUserKeySql = R"(
    SELECT api_key, model, provider
    FROM user_api_keys
    WHERE user_id = $1 AND provider = $2
)";

} // namespace services
