/**
 * @file ConstraintMutCtrl.cpp
 * @brief Constraint add and drop endpoints.
 */

#include "ConstraintMutCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void ConstraintMutCtrl::add(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    auto type = (*j)["type"].asString();
    auto col = (*j)["columnName"].asString();
    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(col)) {
        sendError(cb, "Invalid identifier");
        return;
    }

    std::string sql =
        "ALTER TABLE \"" + tbl + "\" ADD ";
    if (type == "UNIQUE")
        sql += "UNIQUE (\"" + col + "\")";
    else if (type == "NOT NULL")
        sql = "ALTER TABLE \"" + tbl
              + "\" ALTER COLUMN \""
              + col + "\" SET NOT NULL";
    else if (type == "CHECK")
        sql += "CHECK ("
               + (*j)["expression"].asString()
               + ")";
    else {
        sendError(cb,
                  "Unsupported constraint type");
        return;
    }

    try {
        DbPool::get()->execSqlSync(sql);
        Json::Value ok;
        ok["status"] = "added";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

void ConstraintMutCtrl::drop(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    auto name = (*j)["constraintName"].asString();
    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(name)) {
        sendError(cb, "Invalid identifier");
        return;
    }
    try {
        DbPool::get()->execSqlSync(
            "ALTER TABLE \"" + tbl
            + "\" DROP CONSTRAINT \""
            + name + "\"");
        Json::Value ok;
        ok["status"] = "dropped";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
