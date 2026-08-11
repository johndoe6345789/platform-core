/**
 * @file portal_renderer.cpp
 * @brief HTML renderer implementation. The JSON
 *        payload and CSS are embedded into the
 *        binary by cmake/portal.cmake at build
 *        time via configure_file.
 */

#include "portal/backend/portal_renderer.h"
#include "portal/backend/portal_embedded.h"
#include "portal/backend/portal_html_helpers.h"

#include <sstream>
#include <string>

namespace portal
{

std::string renderPortalHtml(
    const nlohmann::json& tools)
{
    std::ostringstream os;
    os << "<!DOCTYPE html>\n<html lang=\"en\">\n"
       << "<head>\n"
       << "  <meta charset=\"utf-8\">\n"
       << "  <meta name=\"viewport\" "
       << "content=\"width=device-width,"
       << " initial-scale=1\">\n"
       << "  <title>Nextra Portal</title>\n"
       << "  <link rel=\"stylesheet\" "
       << "href=\"/portal/styles.css\">\n"
       << "</head>\n<body>\n"
       << "  <header>\n"
       << "    <h1>Nextra Developer Portal</h1>\n"
       << "    <p>All services at a glance</p>\n"
       << "  </header>\n  <div class=\"grid\">\n";
    if (tools.is_array()) {
        for (const auto& t : tools) {
            detail::renderCard(os, t);
        }
    }
    os << "  </div>\n  <footer>\n"
       << "    Nextra &mdash; "
       << "docker compose --profile tools up\n"
       << "  </footer>\n"
       << "  <script>"
       << "function p(){document.querySelectorAll("
       << "'.status-dot').forEach(d=>{"
       << "const u=d.dataset.statusUrl;if(!u)return;"
       << "fetch(u,{method:'GET',cache:'no-store',"
       << "redirect:'follow'}).then(r=>"
       << "d.className='status-dot '+("
       << "r.ok?'up':'down')).catch(()=>"
       << "d.className='status-dot down');});}"
       << "p();setInterval(p,10000);"
       << "</script>\n"
       << "</body>\n</html>\n";
    return os.str();
}

std::string renderPortalHtml()
{
    static const auto tools =
        nlohmann::json::parse(kToolLinksJson);
    return renderPortalHtml(tools);
}

const std::string& portalStylesheet()
{
    static const std::string css{kPortalStylesCss};
    return css;
}

} // namespace portal
