/**
 * @file test_portal_renderer.cpp
 * @brief Unit tests for the portal HTML renderer.
 *
 * The renderer is pure — it takes a JSON array and
 * returns an HTML string. These tests exercise the
 * happy path, empty input, and HTML-escape safety.
 */

#include "portal/backend/portal_renderer.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>

namespace
{

using nlohmann::json;

TEST(PortalRenderer, EmptyArrayRendersShell)
{
    const auto html =
        portal::renderPortalHtml(json::array());
    EXPECT_NE(html.find("<html"), std::string::npos);
    EXPECT_NE(
        html.find("Nextra Developer Portal"),
        std::string::npos);
    EXPECT_EQ(
        html.find("class=\"card\""),
        std::string::npos);
}

TEST(PortalRenderer, RendersOneCardForEachEntry)
{
    json tools = json::array({
        {{"url", "/app"},
         {"label", "Main App"},
         {"emoji", "X"},
         {"port", "frontend :3000"},
         {"description", "Learning."}},
        {{"url", "/db"},
         {"label", "DB"},
         {"emoji", "Y"},
         {"port", "pg :5432"},
         {"description", "Database."}},
    });
    const auto html = portal::renderPortalHtml(tools);
    // Two anchor tags, one per entry.
    size_t count = 0;
    size_t pos = 0;
    const std::string needle = "class=\"card\"";
    while ((pos = html.find(needle, pos))
           != std::string::npos) {
        ++count;
        pos += needle.size();
    }
    EXPECT_EQ(count, 2u);
    EXPECT_NE(html.find("Main App"), std::string::npos);
    EXPECT_NE(html.find("DB"), std::string::npos);
}

TEST(PortalRenderer, EscapesHtmlInUserData)
{
    json tools = json::array({
        {{"url", "/evil"},
         {"label", "<script>alert(1)</script>"},
         {"emoji", ""},
         {"port", "x"},
         {"description", "a & b"}},
    });
    const auto html = portal::renderPortalHtml(tools);
    EXPECT_EQ(
        html.find("<script>"), std::string::npos);
    EXPECT_NE(
        html.find("&lt;script&gt;"),
        std::string::npos);
    EXPECT_NE(
        html.find("a &amp; b"), std::string::npos);
}

} // namespace
