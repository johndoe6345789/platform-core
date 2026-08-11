/**
 * @file TableCtrl.cpp
 * @brief Table listing endpoint.
 */

#include "TableCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"

using namespace drogon;

namespace pgadmin
{

void TableCtrl::list(
    const HttpRequestPtr&,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    try {
        auto rows = DbPool::get()->execSqlSync(
            "SELECT table_name "
            "FROM information_schema.tables "
            "WHERE table_schema='public' "
            "ORDER BY table_name");
        Json::Value arr(Json::arrayValue);
        for (const auto& r : rows) {
            Json::Value obj;
            obj["table_name"] =
                r["table_name"].as<std::string>();
            arr.append(obj);
        }
        Json::Value res;
        res["tables"] = arr;
        sendJson(cb, res);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
