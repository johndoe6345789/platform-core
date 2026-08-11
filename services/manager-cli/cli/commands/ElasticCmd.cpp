/**
 * @file ElasticCmd.cpp
 * @brief Registration of Elasticsearch subcommands.
 */

#include "ElasticCmd.h"

#include <string>

namespace manager
{

void ElasticCmd::registerAll(CLI::App& parent)
{
    auto* es = parent.add_subcommand(
        "es", "Elasticsearch operations");
    es->require_subcommand(1);

    // --- status ---
    es->add_subcommand("status", "Cluster health")
        ->callback([]() { status(); });

    // --- init ---
    es->add_subcommand("init", "Create all indices")
        ->callback([]() { init(); });

    // --- reindex ---
    auto* rex = es->add_subcommand(
        "reindex", "Reindex a service");
    static std::string svc;
    rex->add_option("--service,-s", svc,
                     "nextra|pkgrepo|s3|all")
        ->required();
    rex->callback([]() { reindex(svc); });

    // --- drop ---
    auto* drp = es->add_subcommand(
        "drop", "Drop a specific index");
    static std::string idx;
    drp->add_option("--index,-i", idx,
                     "Index name to delete")
        ->required();
    drp->callback([]() { drop(idx); });
}

} // namespace manager
