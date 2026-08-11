/**
 * @file IndexMutCtrl.cpp
 * @brief Index create and drop endpoints.
 */

#include "IndexMutCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void IndexMutCtrl::create(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    auto name = (*j)["indexName"].asString();
    auto type = (*j)["indexType"].asString();
    bool unique = (*j)["unique"].asBool();

    if (!SqlValidator::isValidIdentifier(tbl) ||
        !SqlValidator::isValidIdentifier(name)) {
        sendError(cb, "Invalid identifier");
        return;
    }

    std::string sql = "CREATE ";
    if (unique) sql += "UNIQUE ";
    sql += "INDEX \"" + name + "\" ON \""
           + tbl + "\"";
    if (!type.empty() && type != "BTREE")
        sql += " USING " + type;
    sql += " (";
    auto& cols = (*j)["columns"];
    for (Json::ArrayIndex i = 0;
         i < cols.size(); ++i) {
        auto c = cols[i].asString();
        if (!SqlValidator::isValidIdentifier(c)) {
            sendError(cb, "Invalid column");
            return;
        }
        if (i > 0) sql += ", ";
        sql += "\"" + c + "\"";
    }
    sql += ")";

    try {
        DbPool::get()->execSqlSync(sql);
        Json::Value ok;
        ok["status"] = "created";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

void IndexMutCtrl::drop(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto name = (*j)["indexName"].asString();
    if (!SqlValidator::isValidIdentifier(name)) {
        sendError(cb, "Invalid index name");
        return;
    }
    try {
        DbPool::get()->execSqlSync(
            "DROP INDEX \"" + name + "\"");
        Json::Value ok;
        ok["status"] = "dropped";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
