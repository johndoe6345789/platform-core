/**
 * @file RecordUpdateCtrl.cpp
 * @brief Record update and delete endpoints.
 */

#include "RecordUpdateCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void RecordUpdateCtrl::update(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    auto pkCol = (*j)["primaryKey"].asString();
    auto pkVal = (*j)["primaryValue"].asString();
    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(pkCol)) {
        sendError(cb, "Invalid identifier");
        return;
    }
    auto& data = (*j)["data"];
    std::string sets;
    for (const auto& key : data.getMemberNames()) {
        if (!SqlValidator::isValidIdentifier(key))
            continue;
        if (!sets.empty()) sets += ", ";
        auto v = data[key].asString();
        std::string esc = "'";
        for (char c : v) {
            if (c == '\'') esc += "''";
            else esc += c;
        }
        sets += "\"" + key + "\" = " + esc + "'";
    }
    auto sql = "UPDATE \"" + tbl + "\" SET "
               + sets + " WHERE \""
               + pkCol + "\" = '" + pkVal + "'";
    try {
        DbPool::get()->execSqlSync(sql);
        Json::Value ok;
        ok["status"] = "updated";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

void RecordUpdateCtrl::remove(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    auto pkCol = (*j)["primaryKey"].asString();
    auto pkVal = (*j)["primaryValue"].asString();
    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(pkCol)) {
        sendError(cb, "Invalid identifier");
        return;
    }
    try {
        DbPool::get()->execSqlSync(
            "DELETE FROM \"" + tbl
            + "\" WHERE \"" + pkCol
            + "\" = '" + pkVal + "'");
        Json::Value ok;
        ok["status"] = "deleted";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
