/**
 * @file safeNext.ts
 * @brief Sanitises the `next` redirect parameter
 * so only same-host paths are ever followed.
 */

/**
 * Must start with / then an alphanumeric char.
 * Prevents protocol-relative URLs (//evil.com),
 * backslash tricks (/\evil), and empty paths.
 */
const SAFE_PATHS = /^\/[a-zA-Z0-9][a-zA-Z0-9/_?=&%-]*$/;

/**
 * Auth pages must never be used as redirect
 * targets — they would cause loops or dead ends.
 */
const AUTH_PATHS =
  /\/(login|register|forgot-password|reset-password)(\/|$|\?)/;

/**
 * Return `raw` if it is a safe, non-auth path,
 * otherwise return the fallback `/`.
 *
 * @param raw - Untrusted value from searchParams.
 * @returns A validated redirect path.
 */
export function safeNext(
  raw: string | undefined,
): string {
  if (
    raw &&
    SAFE_PATHS.test(raw) &&
    !AUTH_PATHS.test(raw)
  ) return raw;
  return '/app/en';
}
