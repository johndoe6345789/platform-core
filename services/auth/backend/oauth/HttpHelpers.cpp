/**
 * @file HttpHelpers.cpp
 * @brief Shared OAuth HTTP helper implementations.
 */

#include "HttpHelpers.h"

#include <cctype>
#include <string>
#include <utility>

namespace services::auth::oauth
{

std::pair<std::string, std::string>
splitUrl(const std::string& url)
{
    const auto pos = url.find("://");
    if (pos == std::string::npos) return {url, "/"};
    const auto a = url.find('/', pos + 3);
    if (a == std::string::npos) return {url, "/"};
    return {url.substr(0, a), url.substr(a)};
}

std::string urlEncode(const std::string& s)
{
    static const char* hex = "0123456789ABCDEF";
    std::string out;
    for (unsigned char c : s) {
        if (std::isalnum(c) || c == '-' || c == '_' ||
            c == '.' || c == '~') {
            out.push_back(static_cast<char>(c));
        } else {
            out.push_back('%');
            out.push_back(hex[c >> 4]);
            out.push_back(hex[c & 0x0f]);
        }
    }
    return out;
}

} // namespace services::auth::oauth
