#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace commands::runtime
{

/**
 * @brief Supported top-level JSON seed files.
 */
enum class SeedKind
{
    users = 0,
    badges = 1,
    featureToggles = 2,
    notifications = 3,
    unsupported = 4,
};

/**
 * @brief Walk up from the current directory to the repo root.
 * @return Absolute repo root path, or empty when not found.
 */
[[nodiscard]] auto repoRoot() -> std::filesystem::path;

/**
 * @brief Resolve the Drogon config path for one-shot commands.
 * @return Existing config path.
 * @throws std::runtime_error when no config file is found.
 */
[[nodiscard]] auto resolveConfigPath() -> std::filesystem::path;

/**
 * @brief Resolve supported seed files to load.
 * @param file Optional explicit seed file path.
 * @return Ordered seed file paths.
 * @throws std::runtime_error on missing or unsupported files.
 */
[[nodiscard]] auto resolveSeedFiles(const std::string& file)
    -> std::vector<std::filesystem::path>;

/**
 * @brief Classify a seed file by filename.
 * @param path Candidate seed file path.
 * @return Matching seed kind or @ref SeedKind::unsupported.
 */
[[nodiscard]] auto seedKind(
    const std::filesystem::path& path) -> SeedKind;

} // namespace commands::runtime
