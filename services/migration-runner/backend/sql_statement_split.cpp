/** @file sql_statement_split.cpp @brief See header. */
#include "migration-runner/backend/sql_statement_split.h"
#include <cctype>

namespace services
{

std::vector<std::string>
splitSqlStatements(const std::string& sql)
{
    std::vector<std::string> out;
    std::string cur;
    const std::size_t n = sql.size();
    std::size_t i = 0;

    auto flush = [&out, &cur]() {
        const auto s = cur.find_first_not_of(" \t\r\n");
        if (s != std::string::npos) {
            out.push_back(cur.substr(s));
        }
        cur.clear();
    };

    while (i < n) {
        const char c = sql[i];

        if (c == '-' && i + 1 < n && sql[i + 1] == '-') {
            while (i < n && sql[i] != '\n') {
                cur += sql[i++];
            }
            continue;
        }
        if (c == '/' && i + 1 < n && sql[i + 1] == '*') {
            cur += sql[i++];
            cur += sql[i++];
            while (i < n) {
                if (sql[i] == '*' && i + 1 < n
                    && sql[i + 1] == '/') {
                    cur += sql[i++];
                    cur += sql[i++];
                    break;
                }
                cur += sql[i++];
            }
            continue;
        }
        if (c == '\'' || c == '"') {
            const char q = c;
            cur += sql[i++];
            while (i < n) {
                const char d = sql[i];
                cur += d;
                ++i;
                if (d == q) {
                    if (i < n && sql[i] == q) {
                        cur += sql[i++];
                        continue;
                    }
                    break;
                }
            }
            continue;
        }
        if (c == '$') {
            std::size_t j = i + 1;
            while (j < n
                   && (sql[j] == '_'
                       || std::isalnum(static_cast<
                              unsigned char>(sql[j])))) {
                ++j;
            }
            if (j < n && sql[j] == '$') {
                const std::string tag =
                    sql.substr(i, j - i + 1);
                const auto end = sql.find(tag, j + 1);
                const std::size_t stop =
                    (end == std::string::npos)
                        ? n
                        : end + tag.size();
                cur += sql.substr(i, stop - i);
                i = stop;
                continue;
            }
        }
        if (c == ';') {
            flush();
            ++i;
            continue;
        }
        cur += c;
        ++i;
    }
    flush();
    return out;
}

} // namespace services
