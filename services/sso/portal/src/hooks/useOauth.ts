'use client';
import { useCallback } from 'react';

/** Supported OAuth provider identifiers. */
export type OauthProvider =
  | 'google'
  | 'github'
  | 'microsoft';

/**
 * Hook returning a function that redirects the browser to
 * the backend's OAuth authorize endpoint for a given
 * provider.  The backend handles PKCE and state creation.
 */
export function useOauth() {
  const start = useCallback((p: OauthProvider) => {
    window.location.href =
      `/api/auth/oauth/${p}/authorize`;
  }, []);
  return { start };
}
