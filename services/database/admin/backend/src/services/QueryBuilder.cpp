/**
 * @file QueryBuilder.cpp
 * @brief Builds parameterized SELECT queries.
 */

#include "QueryBuilder.h"
#include "SqlValidator.h"

#include <sstream>

namespace pgadmin
{

BuildResult QueryBuilder::build(
    const Json::Value& p)
{
    auto tbl = p["table"].asString();
    if (!SqlValidator::isValidIdentifier(tbl))
        return {false, {}, "Invalid table name"};

    std::string cols = "*";
    if (p.isMember("columns") &&
        p["columns"].isArray() &&
        p["columns"].size() > 0) {
        std::ostringstream ss;
        for (Json::ArrayIndex i = 0;
             i < p["columns"].size(); ++i) {
            auto c = p["columns"][i].asString();
            if (!SqlValidator::isValidIdentifier(c))
                return {false, {},
                        "Invalid column: " + c};
            if (i > 0) ss << ", ";
            ss << '"' << c << '"';
        }
        cols = ss.str();
    }

    std::string q =
        "SELECT " + cols + " FROM \"" + tbl + "\"";

    if (p.isMember("where") &&
        p["where"].isArray()) {
        std::vector<std::string> clauses;
        for (const auto& w : p["where"]) {
            auto col = w["column"].asString();
            auto op = w["operator"].asString();
            if (!SqlValidator::isValidIdentifier(col))
                return {false, {},
                        "Invalid WHERE column"};
            std::string cn = "\"" + col + "\"";
            if (op == "IS NULL" ||
                op == "IS NOT NULL") {
                clauses.push_back(cn + " " + op);
            } else {
                clauses.push_back(
                    cn + " " + op + " " +
                    SqlValidator::sanitizeValue(
                        w["value"]));
            }
        }
        if (!clauses.empty()) {
            q += " WHERE ";
            for (size_t i = 0;
                 i < clauses.size(); ++i) {
                if (i > 0) q += " AND ";
                q += clauses[i];
            }
        }
    }

    if (p.isMember("orderBy") &&
        p["orderBy"].isObject()) {
        auto col =
            p["orderBy"]["column"].asString();
        if (!SqlValidator::isValidIdentifier(col))
            return {false, {},
                    "Invalid ORDER BY column"};
        auto dir =
            p["orderBy"]["direction"].asString();
        if (dir != "DESC") dir = "ASC";
        q += " ORDER BY \"" + col + "\" " + dir;
    }

    if (p.isMember("limit"))
        q += " LIMIT " +
             std::to_string(p["limit"].asInt());
    if (p.isMember("offset"))
        q += " OFFSET " +
             std::to_string(p["offset"].asInt());

    return {true, q, {}};
}

} // namespace pgadmin
