/**
 * @file ElasticCmd.h
 * @brief Elasticsearch subcommand hub — registers status,
 *        init, reindex, and drop as nested CLI11 subcommands
 *        under `es`.
 */

#pragma once

#include <CLI/CLI.hpp>

#include <string>

namespace manager
{

/**
 * @class ElasticCmd
 * @brief Registers all Elasticsearch-related subcommands
 *        on the parent CLI::App.
 */
class ElasticCmd
{
  public:
    /// @brief Base URL for the local ES instance.
    static constexpr const char* kBaseUrl =
        "http://localhost:9200";

    /**
     * @brief Register every ES subcommand.
     *
     * Creates `es status|init|reindex|drop` under the
     * given parent app.
     *
     * @param parent The root CLI::App.
     */
    static void registerAll(CLI::App& parent);

    /**
     * @brief Check cluster health.
     * @return int 0 on success, non-zero on failure.
     */
    static int status();

    /**
     * @brief Create all indices for every service.
     * @return int 0 on success, non-zero on failure.
     */
    static int init();

    /**
     * @brief Reindex a specific service or all.
     * @param service One of: nextra, pkgrepo, s3, all.
     * @return int 0 on success, non-zero on failure.
     */
    static int reindex(const std::string& service);

    /**
     * @brief Drop a specific index by name.
     * @param index The ES index name to delete.
     * @return int 0 on success, non-zero on failure.
     */
    static int drop(const std::string& index);
};

} // namespace manager
