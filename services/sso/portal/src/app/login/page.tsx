import { redirect } from 'next/navigation';
import { safeNext } from '@/lib/safeNext';

interface LoginPageProps {
  searchParams: Promise<{ next?: string }>;
}

/**
 * SSO login page.
 *
 * Historically this page rendered a local username/password
 * form. As of the Keycloak migration, the local form is
 * bypassed: the entire identity flow now lives in Keycloak
 * and this page exists only as a thin server-side redirector
 * so external bookmarks / `/sso/login?next=...` links keep
 * working.
 *
 * The original requested URL travels through the OIDC
 * `state` param so the callback can return the user there.
 *
 * The file is intentionally retained (template repo policy:
 * fix in place, never delete features).
 *
 * @param props - Page props with searchParams.
 */
export default async function LoginPage({
  searchParams,
}: LoginPageProps) {
  const { next: raw } = await searchParams;
  const next = safeNext(raw);
  // Bounce to the frontend login page rather than directly
  // to Keycloak — the frontend's `login()` sets up the PKCE
  // verifier+state cookies that the callback page validates.
  // Absolute URL because basePath '/sso' would prepend.
  const target = 'http://localhost:8889'
    + '/app/en/login?next='
    + encodeURIComponent(next);
  redirect(target);
}
