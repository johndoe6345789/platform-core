#pragma once
/**
 * @file RpConfig.h
 * @brief Relying-Party configuration loaded from env vars.
 *
 * Values fall back to developer defaults so tests still run
 * when no env vars are set.
 */

#include <string>

namespace services::auth::passkeys
{

/** @brief Cached relying-party configuration. */
class RpConfig
{
  public:
    /** @brief Process-wide singleton access. */
    static const RpConfig& instance();

    /** @brief WebAuthn RP id (hostname, no scheme). */
    [[nodiscard]] const std::string& id() const;

    /** @brief Human-readable RP name shown by browsers. */
    [[nodiscard]] const std::string& name() const;

    /** @brief Expected origin string for client data. */
    [[nodiscard]] const std::string& origin() const;

  private:
    RpConfig();
    std::string id_;
    std::string name_;
    std::string origin_;
};

} // namespace services::auth::passkeys
