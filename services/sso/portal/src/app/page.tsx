import { redirect } from 'next/navigation';

/**
 * SSO portal landing page.
 *
 * Post-Keycloak migration there is no local UI to land on,
 * so anyone hitting `/sso/` is sent straight to the Keycloak
 * authorize endpoint with the same params used by the
 * nginx `@sso_login` named location.
 *
 * Kept (rather than deleted) per template repo policy:
 * fix in place, never strip features.
 */
export default function SsoIndexPage() {
  // Bounce to frontend login so PKCE cookies are set up
  // before hitting Keycloak. Absolute URL needed because
  // SSO portal basePath '/sso' would prepend a relative one.
  redirect(
    'http://localhost:8889'
    + '/app/en/login?next=%2F',
  );
}
