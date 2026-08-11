/**
 * @file TableMutCtrl.cpp
 * @brief Create and drop table endpoints.
 */

#include "TableMutCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void TableMutCtrl::create(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto json = req->getJsonObject();
    if (!json ||
        !(*json)["tableName"].isString() ||
        !(*json)["columns"].isArray()) {
        sendError(cb,
                  "tableName and columns required");
        return;
    }

    auto tbl = (*json)["tableName"].asString();
    if (!SqlValidator::isValidIdentifier(tbl)) {
        sendError(cb, "Invalid table name");
        return;
    }

    std::string sql =
        "CREATE TABLE \"" + tbl + "\" (";
    auto& cols = (*json)["columns"];
    for (Json::ArrayIndex i = 0;
         i < cols.size(); ++i) {
        auto name = cols[i]["name"].asString();
        auto type = cols[i]["type"].asString();
        if (!SqlValidator::isValidIdentifier(name)) {
            sendError(cb,
                      "Invalid column: " + name);
            return;
        }
        if (i > 0) sql += ", ";
        sql += "\"" + name + "\" " + type;
        if (cols[i]["primaryKey"].asBool())
            sql += " PRIMARY KEY";
        if (cols[i]["notNull"].asBool())
            sql += " NOT NULL";
    }
    sql += ")";

    try {
        DbPool::get()->execSqlSync(sql);
        Json::Value ok;
        ok["status"] = "created";
        ok["table"] = tbl;
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

void TableMutCtrl::drop(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto json = req->getJsonObject();
    if (!json ||
        !(*json)["tableName"].isString()) {
        sendError(cb, "tableName required");
        return;
    }

    auto tbl = (*json)["tableName"].asString();
    if (!SqlValidator::isValidIdentifier(tbl)) {
        sendError(cb, "Invalid table name");
        return;
    }

    try {
        DbPool::get()->execSqlSync(
            "DROP TABLE \"" + tbl + "\"");
        Json::Value ok;
        ok["status"] = "dropped";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
