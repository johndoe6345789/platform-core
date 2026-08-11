/** @file migration_stmt_filter.cpp @brief See header. */
#include "migration-runner/backend/migration_stmt_filter.h"

#include <cctype>

namespace services::migrations
{

bool isTxnCtl(const std::string& s)
{
    const std::size_t n = s.size();
    std::size_t i = 0;
    // Skip leading whitespace + SQL comments so a BEGIN
    // preceded by a file-header comment block is still
    // recognised (else it executes, opens a txn, and one
    // failure aborts the whole file).
    while (i < n) {
        const char c = s[i];
        if (std::isspace(static_cast<unsigned char>(c))) {
            ++i;
        } else if (c == '-' && i + 1 < n && s[i + 1] == '-') {
            while (i < n && s[i] != '\n') {
                ++i;
            }
        } else if (c == '/' && i + 1 < n && s[i + 1] == '*') {
            i += 2;
            while (i + 1 < n
                   && !(s[i] == '*' && s[i + 1] == '/')) {
                ++i;
            }
            i += 2;
        } else {
            break;
        }
    }
    std::string w;
    for (; i < n; ++i) {
        const char c = s[i];
        if (std::isspace(static_cast<unsigned char>(c))
            || c == ';') {
            break;
        }
        w += static_cast<char>(
            std::toupper(static_cast<unsigned char>(c)));
    }
    return w == "BEGIN" || w == "COMMIT" || w == "ROLLBACK"
           || w == "START" || w == "END";
}

bool isBenignDup(const std::string& msg)
{
    return msg.find("already exists") != std::string::npos
           || msg.find("duplicate key value")
                  != std::string::npos;
}

} // namespace services::migrations
