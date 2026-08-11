/**
 * @file ConstraintCtrl.cpp
 * @brief Constraint listing endpoint.
 */

#include "ConstraintCtrl.h"
#include "CtrlUtils.h"
#include "../services/DbPool.h"
#include "../services/SqlValidator.h"

using namespace drogon;

namespace pgadmin
{

void ConstraintCtrl::list(
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
            "SELECT tc.constraint_name, "
            "tc.constraint_type, "
            "kcu.column_name "
            "FROM information_schema"
            ".table_constraints tc "
            "JOIN information_schema"
            ".key_column_usage kcu "
            "ON tc.constraint_name="
            "kcu.constraint_name "
            "WHERE tc.table_schema='public' "
            "AND tc.table_name=$1",
            tbl);
        Json::Value arr(Json::arrayValue);
        for (const auto& r : rows) {
            Json::Value obj;
            obj["constraint_name"] =
                r["constraint_name"]
                    .as<std::string>();
            obj["constraint_type"] =
                r["constraint_type"]
                    .as<std::string>();
            obj["column_name"] =
                r["column_name"]
                    .as<std::string>();
            arr.append(obj);
        }
        Json::Value res;
        res["constraints"] = arr;
        sendJson(cb, res);
    } catch (const std::exception& e) {
        sendServerError(cb, e);
    }
}

} // namespace pgadmin
