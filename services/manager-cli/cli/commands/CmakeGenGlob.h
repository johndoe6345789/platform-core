/**
 * @file CmakeGenGlob.h
 * @brief Glob-to-regex conversion and filename matching
 *        for the embedded cmake-gen functionality.
 *
 * Source discovery is in CmakeGenDiscover.h.
 */

#pragma once

#include <algorithm>
#include <regex>
#include <string>
#include <vector>

namespace manager
{

/// @brief Convert an fnmatch-style glob to std::regex.
inline std::regex glob_to_regex(const std::string& glob)
{
    static const std::string meta = ".()[]{}+^$|\\";
    std::string pat;
    pat.reserve(glob.size() * 2);
    for (char ch : glob) {
        if (ch == '*')
            pat += ".*";
        else if (ch == '?')
            pat += '.';
        else if (meta.find(ch) != std::string::npos) {
            pat += '\\';
            pat += ch;
        } else
            pat += ch;
    }
    return std::regex(pat, std::regex::icase);
}

/// @brief Test if a filename matches any glob pattern.
inline bool matches_any(const std::string& filename,
                        const std::vector<std::string>& patterns)
{
    for (const auto& p : patterns) {
        if (std::regex_match(filename, glob_to_regex(p)))
            return true;
    }
    return false;
}

} // namespace manager
