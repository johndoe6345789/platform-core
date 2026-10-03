/**
 * @file publicOrigin.ts
 * @brief The site's public origin, read at request time.
 *
 * NEXT_PUBLIC_* values are inlined when the image is built, so
 * a deploy-time value has to come from PUBLIC_ORIGIN instead.
 */

/** Public origin without a trailing slash. */
export function publicOrigin(): string {
  const o =
    process.env.PUBLIC_ORIGIN ||
    process.env.NEXT_PUBLIC_ORIGIN ||
    'http://localhost:8889';
  return o.replace(/\/+$/, '');
}
