#pragma once
/**
 * @file portal_renderer.h
 * @brief Pure-function HTML renderer for the
 *        developer portal homepage.
 *
 * Reads the shared tool-links JSON (baked in at
 * build time via configure_file) and emits a
 * complete HTML document listing each tool as
 * a card. No Drogon types here — keeps the
 * renderer trivially unit-testable.
 */

#include <nlohmann/json.hpp>
#include <string>

namespace portal
{

/**
 * @brief Render the portal HTML document.
 * @param tools JSON array matching the schema of
 *              shared/constants/tool-links.json.
 * @return Complete HTML5 document as a string.
 */
std::string renderPortalHtml(
    const nlohmann::json& tools);

/**
 * @brief Render the portal HTML document from the
 *        tool-links JSON baked in at build time.
 * @return Complete HTML5 document as a string.
 */
std::string renderPortalHtml();

/** @brief Portal stylesheet content as a string. */
const std::string& portalStylesheet();

} // namespace portal
