'use client';
import React from 'react';
import { useOauth, OauthProvider } from '@/hooks/useOauth';

const PROVIDERS: ReadonlyArray<{
  id: OauthProvider;
  label: string;
}> = [
  { id: 'google', label: 'Continue with Google' },
  { id: 'github', label: 'Continue with GitHub' },
  { id: 'microsoft', label: 'Continue with Microsoft' },
];

/**
 * OAuth link / sign-in page.  Shows one button per
 * supported provider; clicking begins the backend
 * authorize redirect flow.
 */
export default function OauthLinkPage() {
  const { start } = useOauth();
  return (
    <div className="card" data-testid="oauth-page">
      <p className="logo">NextExtra</p>
      <p className="subtitle">Link a social account</p>
      <div className="sso-links">
        {PROVIDERS.map((p) => (
          <button
            key={p.id}
            type="button"
            className="btn"
            onClick={() => start(p.id)}
            aria-label={p.label}
            data-testid={`oauth-${p.id}`}
          >
            {p.label}
          </button>
        ))}
      </div>
    </div>
  );
}
