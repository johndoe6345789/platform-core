#pragma once
/**
 * @file portal_html_helpers.h
 * @brief Small HTML-escaping and card-rendering
 *        helpers used by portal_renderer.cpp.
 */

#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace portal::detail
{

/** @brief HTML-escape a UTF-8 string for text nodes. */
inline std::string esc(const std::string& s)
{
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        switch (c) {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&#39;";  break;
            default:   out += c;
        }
    }
    return out;
}

/**
 * @brief Append one tool-card anchor to the
 *        output stream.
 * @param os   Output HTML stream.
 * @param t    Single tool-link JSON object.
 */
inline void renderCard(
    std::ostringstream& os,
    const nlohmann::json& t)
{
    const auto url =
        t.value("url", std::string{});
    const auto label =
        t.value("label", std::string{});
    const auto emoji =
        t.value("emoji", std::string{});
    const auto port =
        t.value("port", std::string{});
    const auto desc =
        t.value("description", std::string{});
    const auto statusUrl =
        t.value("statusUrl", url);

    const auto href = url.empty() ? std::string{"/"} :
        (url.back() == '/' ? url : url + "/");
    os << "    <a class=\"card\" href=\""
       << esc(href) << "\">\n"
       << "      <span class=\"status-dot\""
       << " data-status-url=\""
       << esc(statusUrl) << "\""
       << " aria-label=\"service status\"></span>\n"
       << "      <div class=\"icon\">"
       << esc(emoji) << "</div>\n"
       << "      <h2>" << esc(label) << "</h2>\n"
       << "      <p>" << esc(desc) << "</p>\n"
       << "      <span class=\"port\">"
       << esc(port) << "</span>\n"
       << "    </a>\n";
}

} // namespace portal::detail
