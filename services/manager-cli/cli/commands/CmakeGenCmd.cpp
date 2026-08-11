/**
 * @file CmakeGenCmd.cpp
 * @brief Renders inja templates into CMakeLists.txt files.
 */

#include "CmakeGenCmd.h"

#include <fmt/core.h>
#include <inja/inja.hpp>
#include <nlohmann/json.hpp>

#include <filesystem>
#include <fstream>
#include <string>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace manager
{

int CmakeGenCmd::execute(const std::string& input,
                         const std::string& output_dir,
                         const std::string& tmpl_dir)
{
    try {
        fs::path in_abs = fs::absolute(input);
        fs::path base = in_abs.parent_path();
        fs::path out = fs::absolute(output_dir);
        fs::path tmpl = fs::absolute(tmpl_dir);

        fmt::print("[cmake-gen] Reading: {}\n", in_abs.string());

        std::ifstream ifs(in_abs);
        if (!ifs.is_open()) {
            fmt::print("[cmake-gen] Cannot open {}\n", in_abs.string());
            return 1;
        }
        json ctx = buildContext(json::parse(ifs), base);

        std::size_t total = 0;
        for (const auto& t : ctx["targets"]) {
            auto n = t["source_files"].size();
            total += n;
            fmt::print("[cmake-gen] '{}': {} source(s)\n",
                       t["name"].get<std::string>(), n);
        }

        inja::Environment env;
        std::size_t gen = 0;
        for (const auto& e : fs::directory_iterator(tmpl)) {
            if (!e.is_regular_file())
                continue;
            auto fname = e.path().filename().string();
            if (fname.size() < 10 ||
                fname.substr(fname.size() - 9) != ".cmake.j2")
                continue;

            std::ifstream tfs(e.path());
            std::string content((std::istreambuf_iterator<char>(tfs)),
                                std::istreambuf_iterator<char>());

            auto rendered = env.render(content, ctx);
            auto out_name = (fname == "root.cmake.j2")
                                ? "CMakeLists.txt"
                                : fname.substr(0, fname.size() - 3);

            fs::path dest = out / out_name;
            fs::create_directories(dest.parent_path());
            std::ofstream ofs(dest);
            ofs << rendered;
            ++gen;
            fmt::print("[cmake-gen] Generated: {}\n", dest.string());
        }
        fmt::print("[cmake-gen] {} source(s), {} "
                   "file(s)\n",
                   total, gen);
    } catch (const std::exception& ex) {
        fmt::print("[cmake-gen] Error: {}\n", ex.what());
        return 1;
    }
    return 0;
}

} // namespace manager
