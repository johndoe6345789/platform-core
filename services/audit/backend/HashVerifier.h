#pragma once

/**
 * @file HashVerifier.h
 * @brief Walks the `audit_events` hash chain for a tenant.
 */

#include "AuditTypes.h"

#include <drogon/orm/DbClient.h>

#include <memory>
#include <string>

namespace nextra::audit
{

class HashVerifier
{
public:
    /**
     * @brief Construct with a Drogon DB client.
     * @param db Shared Postgres client.
     * @param pageSize Rows per scan page.
     */
    HashVerifier(drogon::orm::DbClientPtr db, int pageSize);

    /**
     * @brief Verify the chain for a single tenant.
     * @param tenantId UUID string.
     * @return Integrity report — ok=false on first mismatch.
     */
    IntegrityReport verify(const std::string& tenantId);

private:
    drogon::orm::DbClientPtr db_;
    int pageSize_;
};

}  // namespace nextra::audit
