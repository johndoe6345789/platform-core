#pragma once
/**
 * @file StateStore.h
 * @brief Postgres-backed CSRF state store for OAuth flows.
 *
 * Writes to the oauth_states table defined in migration 015.
 * TTLs are enforced at consume() time — rows older than
 * stateTtlSeconds are rejected (the DELETE .. RETURNING query
 * itself filters on created_at).
 */

#include <functional>
#include <optional>
#include <string>

namespace services::auth::oauth
{

/** @brief Record in the oauth_states table. */
struct StoredState
{
    std::string state;
    std::string nonce;
    std::string redirectUri;
    std::string provider;
    std::string codeVerifier;
};

/** @brief Insert a new pending state row. */
void putState(
    const StoredState& s,
    std::function<void(bool)> done);

/**
 * @brief Consume a state row by value; invokes cb(opt).
 *        cb is called with nullopt if absent, expired, or
 *        the row was already consumed.
 */
void consumeState(
    const std::string& state,
    int ttlSeconds,
    std::function<void(std::optional<StoredState>)> cb);

} // namespace services::auth::oauth
