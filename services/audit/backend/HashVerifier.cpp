/**
 * @file HashVerifier.cpp
 * @brief Implementation of hash-chain integrity walker.
 */

#include "audit/backend/HashVerifier.h"
#include "audit/backend/HashChain.h"

#include <nlohmann/json.hpp>

namespace nextra::audit
{

HashVerifier::HashVerifier(drogon::orm::DbClientPtr db,
                           int pageSize)
    : db_(std::move(db)), pageSize_(pageSize)
{
}

IntegrityReport HashVerifier::verify(
    const std::string& tenantId)
{
    IntegrityReport out;
    const char* sql =
        "SELECT id, actor_id, action, target_type, target_id, "
        " payload, encode(prev_hash, 'hex'), "
        " encode(hash, 'hex'), "
        " to_char(created_at, "
        "   'YYYY-MM-DD\"T\"HH24:MI:SS.MS\"Z\"') "
        "FROM audit_events WHERE tenant_id = $1 "
        "ORDER BY created_at ASC, id ASC";
    auto r = db_->execSqlSync(sql, tenantId);
    std::vector<unsigned char> prev;
    for (const auto& row : r) {
        AuditEvent ev;
        ev.tenantId  = tenantId;
        if (!row[1].isNull())
            ev.actorId = row[1].as<std::string>();
        ev.action = row[2].as<std::string>();
        if (!row[3].isNull())
            ev.targetType = row[3].as<std::string>();
        if (!row[4].isNull())
            ev.targetId = row[4].as<std::string>();
        ev.payload = nlohmann::json::parse(
            row[5].as<std::string>(), nullptr, false);
        ev.createdAt = row[8].as<std::string>();

        auto expected = computeHash(prev, ev);
        auto actualHex = row[7].as<std::string>();
        if (toHex(expected) != actualHex) {
            out.ok = false;
            out.firstBadId = row[0].as<std::int64_t>();
            out.message = "hash mismatch at id " +
                          std::to_string(*out.firstBadId);
            return out;
        }
        prev = fromHex(actualHex);
        ++out.rowsChecked;
    }
    out.message = "ok";
    return out;
}

}  // namespace nextra::audit
