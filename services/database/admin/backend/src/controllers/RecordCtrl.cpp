/**
 * @file RecordCtrl.cpp
 * @brief Record insert endpoint.
 */

#include "RecordCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void RecordCtrl::insert(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto j = req->getJsonObject();
    if (!j) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*j)["tableName"].asString();
    if (!SqlValidator::isValidIdentifier(tbl)) {
        sendError(cb, "Invalid table name");
        return;
    }
    auto& data = (*j)["data"];
    std::string cols, vals;
    for (const auto& key : data.getMemberNames()) {
        if (!SqlValidator::isValidIdentifier(key)) {
            sendError(cb,
                      "Invalid column: " + key);
            return;
        }
        if (!cols.empty()) {
            cols += ", ";
            vals += ", ";
        }
        cols += "\"" + key + "\"";
        auto v = data[key].asString();
        std::string esc = "'";
        for (char c : v) {
            if (c == '\'') esc += "''";
            else esc += c;
        }
        vals += esc + "'";
    }
    auto sql = "INSERT INTO \"" + tbl
               + "\" (" + cols + ") VALUES ("
               + vals + ")";
    try {
        DbPool::get()->execSqlSync(sql);
        Json::Value ok;
        ok["status"] = "inserted";
        sendJson(cb, ok);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
