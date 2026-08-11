#pragma once

/**
 * @file AuditTypes.h
 * @brief Plain-data types for the audit-manager daemon.
 *
 * These mirror the columns of `audit_events` plus the envelope
 * format used on the Kafka topic `audit.events`.  Nothing in this
 * header depends on Drogon so the hash-chain verifier can be unit
 * tested in isolation.
 */

#include <nlohmann/json.hpp>

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace nextra::audit
{

/** @brief Decoded Kafka envelope for a single audit event. */
struct AuditEvent
{
    std::string tenantId;       ///< UUID string
    std::optional<std::string> actorId;
    std::string action;         ///< e.g. "auth.login"
    std::optional<std::string> targetType;
    std::optional<std::string> targetId;
    nlohmann::json payload = nlohmann::json::object();
    std::string createdAt;      ///< ISO-8601, set by producer
};

/** @brief One persisted row returned by the controller. */
struct AuditRow
{
    std::int64_t id = 0;
    std::string tenantId;
    std::optional<std::string> actorId;
    std::string action;
    std::optional<std::string> targetType;
    std::optional<std::string> targetId;
    nlohmann::json payload;
    std::string prevHashHex;    ///< empty for chain head
    std::string hashHex;
    std::string createdAt;
};

/** @brief Result of walking the hash chain for a tenant. */
struct IntegrityReport
{
    bool ok = true;
    std::int64_t rowsChecked = 0;
    std::optional<std::int64_t> firstBadId;
    std::string message;
};

}  // namespace nextra::audit
