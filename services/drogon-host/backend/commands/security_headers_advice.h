#pragma once
/**
 * @file security_headers_advice.h
 * @brief Pre-sending advice that appends security headers
 *        (CSP, X-Frame-Options, etc.) to every response.
 */

namespace commands
{

/**
 * @brief Load security-headers.json and register a
 *        Drogon pre-sending advice that stamps the
 *        headers on every outgoing HTTP response.
 */
void registerSecurityHeadersAdvice();

} // namespace commands
