/**
 * @file TableSchemaCtrl.cpp
 * @brief Table schema introspection endpoint.
 */

#include "TableSchemaCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void TableSchemaCtrl::schema(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto json = req->getJsonObject();
    if (!json) {
        sendError(cb, "Missing body");
        return;
    }
    auto tbl = (*json)["tableName"].asString();
    if (!SqlValidator::isValidIdentifier(tbl)) {
        sendError(cb, "Invalid table name");
        return;
    }
    try {
        auto cols = DbPool::get()->execSqlSync(
            "SELECT column_name, data_type, "
            "character_maximum_length, "
            "is_nullable, column_default "
            "FROM information_schema.columns "
            "WHERE table_schema='public' "
            "AND table_name=$1 "
            "ORDER BY ordinal_position",
            tbl);
        auto pks = DbPool::get()->execSqlSync(
            "SELECT kcu.column_name "
            "FROM information_schema"
            ".table_constraints tc "
            "JOIN information_schema"
            ".key_column_usage kcu "
            "ON tc.constraint_name="
            "kcu.constraint_name "
            "AND tc.table_schema="
            "kcu.table_schema "
            "WHERE tc.constraint_type="
            "'PRIMARY KEY' "
            "AND tc.table_schema='public' "
            "AND tc.table_name=$1",
            tbl);
        Json::Value colArr(Json::arrayValue);
        for (const auto& r : cols) {
            Json::Value c;
            c["column_name"] =
                r["column_name"].as<std::string>();
            c["data_type"] =
                r["data_type"].as<std::string>();
            c["is_nullable"] =
                r["is_nullable"].as<std::string>();
            if (!r["column_default"].isNull())
                c["column_default"] =
                    r["column_default"]
                        .as<std::string>();
            colArr.append(c);
        }
        Json::Value pkArr(Json::arrayValue);
        for (const auto& r : pks)
            pkArr.append(
                r["column_name"].as<std::string>());
        Json::Value res;
        res["columns"] = colArr;
        res["primaryKeys"] = pkArr;
        sendJson(cb, res);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
