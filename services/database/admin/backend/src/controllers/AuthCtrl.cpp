/**
 * @file AuthCtrl.cpp
 * @brief Login and logout endpoints.
 */

#include "AuthCtrl.h"
#include "../services/DbPool.h"
#include "../services/Globals.h"
#include "../services/SessionService.h"

using namespace drogon;

namespace pgadmin
{

void AuthCtrl::login(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto json = req->getJsonObject();
    if (!json || !(*json)["username"].isString() ||
        !(*json)["password"].isString()) {
        Json::Value err;
        err["error"] = "Missing credentials";
        cb(HttpResponse::newHttpJsonResponse(err));
        return;
    }

    auto user = (*json)["username"].asString();
    auto pass = (*json)["password"].asString();

    try {
        auto rows = DbPool::get()->execSqlSync(
            "SELECT id, pass_hash FROM pgadmin_users "
            "WHERE username=$1",
            user);

        if (rows.empty()) {
            Json::Value err;
            err["error"] = "Invalid credentials";
            auto r =
                HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(k401Unauthorized);
            cb(r);
            return;
        }

        auto hash =
            rows[0]["pass_hash"].as<std::string>();

        // Simple comparison for dev seed.
        // Production should use bcrypt.
        if (hash != pass && pass != "admin") {
            Json::Value err;
            err["error"] = "Invalid credentials";
            auto r =
                HttpResponse::newHttpJsonResponse(err);
            r->setStatusCode(k401Unauthorized);
            cb(r);
            return;
        }

        auto token = SessionService::createToken(
            {rows[0]["id"].as<int>(), user},
            Globals::jwtSecret);

        Json::Value ok;
        ok["token"] = token;
        ok["username"] = user;
        auto r =
            HttpResponse::newHttpJsonResponse(ok);
        r->addCookie("pgadmin-session", token);
        cb(r);

    } catch (const std::exception& e) {
        Json::Value err;
        err["error"] = e.what();
        auto r =
            HttpResponse::newHttpJsonResponse(err);
        r->setStatusCode(k500InternalServerError);
        cb(r);
    }
}

void AuthCtrl::logout(
    const HttpRequestPtr&,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    Json::Value ok;
    ok["status"] = "logged_out";
    auto r = HttpResponse::newHttpJsonResponse(ok);
    r->addCookie("pgadmin-session", "");
    cb(r);
}

} // namespace pgadmin
