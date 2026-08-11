#pragma once

/**
 * @file AuditWriter.h
 * @brief Persists audit events into the partitioned Postgres log.
 *
 * Each call to `append` looks up the previous row's hash for the
 * same tenant, computes the new chain link, and inserts a row.
 * The writer is intentionally thin — no batching, no dedup — so
 * the consumer can reason about exactly-once semantics at the
 * Kafka level (commit offset after write).
 */

#include "AuditTypes.h"

#include <drogon/orm/DbClient.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace nextra::audit
{

class AuditWriter
{
public:
    /**
     * @brief Construct with an existing Drogon DB client.
     * @param db Shared Drogon DB client (Postgres).
     */
    explicit AuditWriter(drogon::orm::DbClientPtr db);

    /**
     * @brief Append a single event to `audit_events`.
     * @param ev Decoded Kafka payload.
     * @return The generated row id.
     * @throws drogon::orm::DrogonDbException on insert failure.
     */
    std::int64_t append(const AuditEvent& ev);

    /**
     * @brief Fetch the most recent hash for a tenant, or empty.
     * @param tenantId UUID string.
     * @return Raw bytes of the previous hash, empty if none.
     */
    std::vector<unsigned char>
    previousHash(const std::string& tenantId);

    /**
     * @brief List recent events for a tenant within a time range.
     * @param tenantId UUID.
     * @param since    Lower bound (inclusive, ISO-8601).
     * @param until    Upper bound (exclusive, ISO-8601).
     * @param limit    Max rows to return.
     * @return Rows newest-first.
     */
    std::vector<AuditRow> list(const std::string& tenantId,
                               const std::string& since,
                               const std::string& until,
                               int limit);

private:
    drogon::orm::DbClientPtr db_;
};

}  // namespace nextra::audit
