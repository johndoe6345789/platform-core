/**
 * @file QueryCtrl.cpp
 * @brief Raw SQL and query builder endpoints.
 */

#include "QueryCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/QueryBuilder.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

/// @brief Convert result rows to a JSON array.
static Json::Value rowsToJson(
    const drogon::orm::Result& rows)
{
    Json::Value arr(Json::arrayValue);
    for (const auto& r : rows) {
        Json::Value obj;
        for (size_t i = 0; i < r.size(); ++i) {
            auto nm = rows.columnName(i);
            obj[nm] = r[i].isNull()
                ? Json::nullValue
                : Json::Value(
                      r[i].as<std::string>());
        }
        arr.append(obj);
    }
    return arr;
}

void QueryCtrl::raw(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto json = req->getJsonObject();
    if (!json || !(*json)["query"].isString()) {
        sendError(cb, "Query is required");
        return;
    }
    auto query = (*json)["query"].asString();
    if (!SqlValidator::isSelectOnly(query)) {
        sendError(cb,
                  "Only SELECT queries allowed");
        return;
    }
    try {
        auto rows =
            DbPool::get()->execSqlSync(query);
        Json::Value res;
        res["rows"] = rowsToJson(rows);
        res["rowCount"] =
            static_cast<int>(rows.size());
        sendJson(cb, res);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

void QueryCtrl::build(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto json = req->getJsonObject();
    if (!json) {
        sendError(cb, "Missing body");
        return;
    }
    auto result = QueryBuilder::build(*json);
    if (!result.ok) {
        sendError(cb, result.error);
        return;
    }
    try {
        auto rows = DbPool::get()->execSqlSync(
            result.query);
        Json::Value res;
        res["query"] = result.query;
        res["rows"] = rowsToJson(rows);
        res["rowCount"] =
            static_cast<int>(rows.size());
        sendJson(cb, res);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
