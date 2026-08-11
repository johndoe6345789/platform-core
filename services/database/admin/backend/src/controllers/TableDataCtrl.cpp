/**
 * @file TableDataCtrl.cpp
 * @brief Table data read endpoint.
 */

#include "TableDataCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void TableDataCtrl::data(
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
        auto sql =
            "SELECT * FROM \"" + tbl
            + "\" LIMIT 100";
        auto rows =
            DbPool::get()->execSqlSync(sql);
        Json::Value arr(Json::arrayValue);
        for (const auto& r : rows) {
            Json::Value obj;
            for (size_t i = 0;
                 i < r.size(); ++i) {
                auto nm = rows.columnName(i);
                obj[nm] = r[i].isNull()
                    ? Json::nullValue
                    : Json::Value(
                        r[i].as<std::string>());
            }
            arr.append(obj);
        }
        Json::Value res;
        res["rows"] = arr;
        res["rowCount"] =
            static_cast<int>(rows.size());
        sendJson(cb, res);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
