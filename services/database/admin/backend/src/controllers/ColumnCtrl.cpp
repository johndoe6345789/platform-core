/**
 * @file ColumnCtrl.cpp
 * @brief Add column endpoint.
 */

#include "ColumnCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void ColumnCtrl::add(
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
    auto type = (*j)["dataType"].asString();
    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(col)) {
        sendError(cb, "Invalid identifier");
        return;
    }
    auto sql = "ALTER TABLE \"" + tbl
               + "\" ADD COLUMN \""
               + col + "\" " + type;
    if ((*j)["notNull"].asBool())
        sql += " NOT NULL";
    if ((*j)["default"].isString())
        sql += " DEFAULT "
               + (*j)["default"].asString();

    try {
        DbPool::get()->execSqlSync(sql);
        Json::Value ok;
        ok["status"] = "added";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
