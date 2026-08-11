/**
 * @file IndexCtrl.cpp
 * @brief Index listing endpoint.
 */

#include "IndexCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void IndexCtrl::list(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto tbl = req->getParameter("table");
    if (!SqlValidator::isValidIdentifier(tbl)) {
        sendError(cb, "Invalid table name");
        return;
    }
    try {
        auto rows = DbPool::get()->execSqlSync(
            "SELECT indexname, indexdef "
            "FROM pg_indexes "
            "WHERE schemaname='public' "
            "AND tablename=$1",
            tbl);
        Json::Value arr(Json::arrayValue);
        for (const auto& r : rows) {
            Json::Value obj;
            obj["indexname"] =
                r["indexname"].as<std::string>();
            obj["indexdef"] =
                r["indexdef"].as<std::string>();
            arr.append(obj);
        }
        Json::Value res;
        res["indexes"] = arr;
        sendJson(cb, res);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
