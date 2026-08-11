#pragma once
/**
 * @file HttpHelpers.h
 * @brief Tiny shared helpers for OAuth HTTP clients.
 */

#include <string>
#include <utility>

namespace services::auth::oauth
{

/** @brief Split a full URL into (scheme://host, path). */
[[nodiscard]] std::pair<std::string, std::string>
splitUrl(const std::string& url);

/** @brief RFC 3986 unreserved-char URL encoder. */
[[nodiscard]] std::string urlEncode(const std::string& s);

} // namespace services::auth::oauth
