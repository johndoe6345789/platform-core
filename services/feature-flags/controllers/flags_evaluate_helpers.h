#pragma once
/**
 * @file flags_evaluate_helpers.h
 * @brief Helpers for the public evaluate endpoint —
 *        CSV split + batch FlagMap construction. Kept
 *        in a header so FlagsControllerPublic.cpp
 *        stays under the 100 LoC cap.
 */

#include "feature-flags/backend/FlagEvaluator.h"
#include "feature-flags/backend/FlagStore.h"

#include <nlohmann/json.hpp>

#include <sstream>
#include <string>
#include <vector>

namespace controllers::flags_eval
{

/** Split "a,b,,c" → ["a","b","c"], dropping empties. */
inline std::vector<std::string> splitCsv(
    const std::string& s)
{
    std::vector<std::string> out;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) out.push_back(item);
    }
    return out;
}

/**
 * Batch path: ?names=a,b,c → {a:bool, b:bool, c:bool}.
 * Unknown flags resolve to false so the consumer always
 * gets a key for every name it asked about.
 */
inline nlohmann::json evaluateBatch(
    const std::string& namesCsv,
    const std::string& userId)
{
    services::flags::FlagStore store;
    nlohmann::json result = nlohmann::json::object();
    for (const auto& name : splitCsv(namesCsv)) {
        auto flag = store.findByKey(name);
        if (!flag) { result[name] = false; continue; }
        auto ev =
            services::flags::FlagEvaluator::evaluate(
                *flag, userId);
        result[name] = ev.enabled;
    }
    return result;
}

}  // namespace controllers::flags_eval
