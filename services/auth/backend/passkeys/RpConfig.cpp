/**
 * @file RpConfig.cpp
 * @brief Env-var backed WebAuthn relying-party config.
 */

#include "RpConfig.h"

#include <cstdlib>
#include <string>

namespace services::auth::passkeys
{

namespace
{

std::string envOr(const char* key, const char* fallback)
{
    const char* v = std::getenv(key);
    return std::string(v ? v : fallback);
}

} // namespace

RpConfig::RpConfig()
    : id_(envOr("NEXTRA_RP_ID", "localhost")),
      name_(envOr("NEXTRA_RP_NAME", "Nextra")),
      origin_(envOr(
          "NEXTRA_RP_ORIGIN", "http://localhost:8889"))
{
}

const RpConfig& RpConfig::instance()
{
    static RpConfig cfg;
    return cfg;
}

const std::string& RpConfig::id() const { return id_; }
const std::string& RpConfig::name() const { return name_; }
const std::string& RpConfig::origin() const
{
    return origin_;
}

} // namespace services::auth::passkeys
