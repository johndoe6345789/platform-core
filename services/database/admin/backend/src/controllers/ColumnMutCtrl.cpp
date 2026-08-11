/**
 * @file ColumnMutCtrl.cpp
 * @brief Column modify and drop endpoints.
 */

#include "ColumnMutCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void ColumnMutCtrl::modify(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    auto col = (*j)["columnName"].asString();
    auto type = (*j)["newType"].asString();
    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(col)) {
        sendError(cb, "Invalid identifier");
        return;
    }
    auto sql = "ALTER TABLE \"" + tbl
               + "\" ALTER COLUMN \""
               + col + "\" TYPE " + type;
    try {
        DbPool::get()->execSqlSync(sql);
        Json::Value ok;
        ok["status"] = "modified";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

void ColumnMutCtrl::drop(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    auto col = (*j)["columnName"].asString();
    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(col)) {
        sendError(cb, "Invalid identifier");
        return;
    }
    try {
        DbPool::get()->execSqlSync(
            "ALTER TABLE \"" + tbl
            + "\" DROP COLUMN \"" + col + "\"");
        Json::Value ok;
        ok["status"] = "dropped";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
