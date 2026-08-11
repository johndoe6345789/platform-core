/**
 * @file seed_io.cpp
 * @brief Shared JSON file loading for seed commands.
 */

#include "seed_io.h"

#include <fstream>
#include <stdexcept>

namespace commands::seed
{

auto readSeedArray(const std::filesystem::path& path) -> json
{
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error("Cannot open " + path.string());
    auto parsed = json::parse(file);
    if (!parsed.is_array())
        throw std::runtime_error(
            "Seed file must contain an array: " + path.string());
    return parsed;
}

} // namespace commands::seed
