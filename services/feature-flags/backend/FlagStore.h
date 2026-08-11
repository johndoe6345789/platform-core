#pragma once
/**
 * @file FlagStore.h
 * @brief Persistence layer for feature flags (Drogon ORM).
 */

#include "FlagTypes.h"

#include <drogon/orm/Result.h>
#include <optional>
#include <string>
#include <vector>

namespace services::flags
{

/**
 * @brief Synchronous CRUD over the feature_flags table.
 *
 * All calls run on the default Drogon db client. Audit rows
 * are inserted inline when updates occur.
 */
class FlagStore
{
  public:
    /** @brief List all flags ordered by key. */
    std::vector<FeatureFlag> listAll();

    /** @brief Fetch a single flag by key, if any. */
    std::optional<FeatureFlag>
    findByKey(const std::string& key);

    /** @brief Apply a patch; writes an audit row. */
    std::optional<FeatureFlag>
    update(const std::string& key,
           const FlagPatch& patch,
           const std::string& actorId);

    /** @brief Fetch audit rows for a flag, newest first. */
    nlohmann::json
    audit(std::int64_t flagId, int limit);

  private:
    FeatureFlag rowToFlag(
        const drogon::orm::Row& row) const;
};

} // namespace services::flags
