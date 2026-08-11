/**
 * @file passkey_assert_helpers.cpp
 * @brief Session mint + last-used update for passkeys.
 */

#include "passkey_assert_helpers.h"
#include "drogon-host/backend/utils/JsonResponse.h"
#include "drogon-host/backend/utils/JwtUtil.h"

#include <drogon/Cookie.h>
#include <drogon/HttpAppFramework.h>
#include <drogon/orm/DbClient.h>
#include <nlohmann/json.hpp>

namespace controllers::passkey_assert
{

using json = nlohmann::json;

void issuePasskeySession(
    const std::string& userId,
    const std::string& credIdHex,
    std::function<void(
        const drogon::HttpResponsePtr&)> cb)
{
    const auto role = std::string("user");
    auto access =
        ::utils::generateAccessToken(userId, role);
    auto refresh =
        ::utils::generateRefreshToken(userId);
    json payload = {
        {"ok", true},
        {"userId", userId},
        {"tokens",
         {{"accessToken", access},
          {"refreshToken", refresh}}}};
    auto resp = ::utils::jsonOk(payload);
    drogon::Cookie sso("nextra_sso", refresh);
    sso.setHttpOnly(true);
    sso.setPath("/");
    sso.setSameSite(drogon::Cookie::SameSite::kLax);
    sso.setMaxAge(30 * 24 * 3600);
    resp->addCookie(sso);
    auto db = drogon::app().getDbClient();
    db->execSqlAsync(
        "UPDATE passkey_credentials"
        " SET last_used_at = NOW()"
        " WHERE credential_id = decode($1,'hex')",
        [](const drogon::orm::Result&) {},
        [](const drogon::orm::DrogonDbException&) {},
        credIdHex);
    cb(resp);
}

} // namespace controllers::passkey_assert
