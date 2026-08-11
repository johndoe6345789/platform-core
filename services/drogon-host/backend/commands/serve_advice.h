#pragma once
/**
 * @file serve_advice.h
 * @brief Drogon HTTP advice registrations
 *        (request IDs, CORS headers).
 */

namespace commands
{

/**
 * @brief Register all pre-handling and pre-sending
 *        advice on the Drogon application instance.
 */
void registerHttpAdvice();

} // namespace commands
