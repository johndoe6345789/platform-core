#pragma once

/**
 * @file audit_manager.h
 * @brief CLI subcommand for the audit-manager daemon.
 *
 * Consumes the Kafka topic `audit.events`, appends each row to
 * the partitioned Postgres log with a hash chain per tenant,
 * and serves the `/api/audit/*` read endpoints over HTTP.
 */

#include <string>

namespace commands
{

/**
 * @brief Entry point for `nextra-api audit-manager`.
 * @param config Path to the Drogon JSON config file.
 */
void cmdAuditManager(const std::string& config);

}  // namespace commands
