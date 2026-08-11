/**
 * @file AuditWriter.cpp
 * @brief Postgres implementation of AuditWriter.
 */

#include "audit/backend/AuditWriter.h"
#include "audit/backend/HashChain.h"

#include <drogon/orm/Exception.h>
#include <spdlog/spdlog.h>

#include <string>

namespace nextra::audit
{

AuditWriter::AuditWriter(drogon::orm::DbClientPtr db)
    : db_(std::move(db))
{
}

std::vector<unsigned char>
AuditWriter::previousHash(const std::string& tenantId)
{
    const char* sql =
        "SELECT encode(hash, 'hex') "
        "FROM audit_events WHERE tenant_id = $1 "
        "ORDER BY created_at DESC, id DESC LIMIT 1";
    auto r = db_->execSqlSync(sql, tenantId);
    if (r.empty()) return {};
    return fromHex(r[0][0].as<std::string>());
}

std::int64_t AuditWriter::append(const AuditEvent& ev)
{
    auto prev = previousHash(ev.tenantId);
    auto hash = computeHash(prev, ev);
    std::string prevHex = toHex(prev);
    std::string hashHex = toHex(hash);
    std::string payloadStr = ev.payload.dump();

    const char* sql =
        "INSERT INTO audit_events "
        "(tenant_id, actor_id, action, target_type, target_id, "
        " payload, prev_hash, hash, created_at) "
        "VALUES ($1, $2, $3, $4, $5, $6::jsonb, "
        "        decode($7, 'hex'), decode($8, 'hex'), "
        "        COALESCE(NULLIF($9, '')::timestamptz, now())) "
        "RETURNING id";

    auto r = db_->execSqlSync(
        sql, ev.tenantId,
        ev.actorId.value_or(std::string{}),
        ev.action,
        ev.targetType.value_or(std::string{}),
        ev.targetId.value_or(std::string{}),
        payloadStr, prevHex, hashHex, ev.createdAt);
    return r[0][0].as<std::int64_t>();
}

std::vector<AuditRow>
AuditWriter::list(const std::string& tenantId,
                  const std::string& since,
                  const std::string& until, int limit)
{
    const char* sql =
        "SELECT id, tenant_id, actor_id, action, "
        " target_type, target_id, payload, "
        " encode(prev_hash, 'hex'), encode(hash, 'hex'), "
        " to_char(created_at, "
        "   'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"') "
        "FROM audit_events WHERE tenant_id = $1 "
        "  AND created_at >= $2::timestamptz "
        "  AND created_at <  $3::timestamptz "
        "ORDER BY created_at DESC, id DESC LIMIT $4";
    auto r = db_->execSqlSync(sql, tenantId, since, until, limit);
    std::vector<AuditRow> out;
    out.reserve(r.size());
    for (const auto& row : r) {
        AuditRow a;
        a.id = row[0].as<std::int64_t>();
        a.tenantId = row[1].as<std::string>();
        if (!row[2].isNull())
            a.actorId = row[2].as<std::string>();
        a.action = row[3].as<std::string>();
        if (!row[4].isNull())
            a.targetType = row[4].as<std::string>();
        if (!row[5].isNull())
            a.targetId = row[5].as<std::string>();
        a.payload = nlohmann::json::parse(
            row[6].as<std::string>(), nullptr, false);
        a.prevHashHex = row[7].as<std::string>();
        a.hashHex = row[8].as<std::string>();
        a.createdAt = row[9].as<std::string>();
        out.push_back(std::move(a));
    }
    return out;
}

}  // namespace nextra::audit
