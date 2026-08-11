/**
 * @file StateStore.cpp
 * @brief StateStore implementation over Drogon ORM.
 */

#include "StateStore.h"

#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>

#include <optional>
#include <utility>

namespace services::auth::oauth
{

void putState(
    const StoredState& s,
    std::function<void(bool)> done)
{
    auto db = drogon::app().getDbClient();
    db->execSqlAsync(
        "INSERT INTO oauth_states "
        "(state,nonce,redirect_uri,provider,code_verifier)"
        " VALUES ($1,$2,$3,$4,$5)",
        [done](const drogon::orm::Result&) { done(true); },
        [done](const drogon::orm::DrogonDbException&) {
            done(false);
        },
        s.state, s.nonce, s.redirectUri,
        s.provider, s.codeVerifier);
}

void consumeState(
    const std::string& state,
    int ttlSeconds,
    std::function<void(std::optional<StoredState>)> cb)
{
    auto db = drogon::app().getDbClient();
    db->execSqlAsync(
        "DELETE FROM oauth_states WHERE state=$1"
        " AND consumed_at IS NULL"
        " AND created_at > now() - ($2::text"
        " || ' seconds')::interval"
        " RETURNING state, nonce, redirect_uri,"
        " provider, code_verifier",
        [cb](const drogon::orm::Result& r) {
            if (r.empty()) {
                cb(std::nullopt);
                return;
            }
            StoredState s;
            s.state = r[0]["state"].as<std::string>();
            s.nonce = r[0]["nonce"].as<std::string>();
            s.redirectUri =
                r[0]["redirect_uri"].as<std::string>();
            s.provider =
                r[0]["provider"].as<std::string>();
            s.codeVerifier =
                r[0]["code_verifier"].as<std::string>();
            cb(std::move(s));
        },
        [cb](const drogon::orm::DrogonDbException&) {
            cb(std::nullopt);
        },
        state, std::to_string(ttlSeconds));
}

} // namespace services::auth::oauth
