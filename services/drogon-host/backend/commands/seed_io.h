#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

namespace commands::seed
{

using json = nlohmann::json;

/**
 * @brief Read and parse an array-based seed JSON file.
 * @param path Seed file path.
 * @return Parsed JSON array.
 * @throws std::runtime_error on I/O errors or wrong shape.
 */
[[nodiscard]] auto readSeedArray(
    const std::filesystem::path& path) -> json;

} // namespace commands::seed
