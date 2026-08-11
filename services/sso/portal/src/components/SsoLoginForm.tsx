'use client';
import React, { useState } from 'react';

/** Shape of the login API response. */
interface LoginPayload {
  user?: { role?: string };
}

/** Props for SsoLoginForm. */
interface SsoLoginFormProps {
  /** Validated redirect target after login. */
  next: string;
}

/**
 * Login form: POSTs to /api/auth/login, writes
 * tokens to localStorage, then redirects to next.
 */
export default function SsoLoginForm({
  next,
}: SsoLoginFormProps) {
  const [email, setEmail] = useState('');
  const [pw, setPw] = useState('');
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState('');

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    setBusy(true);
    setError('');
    try {
      const res = await fetch('/api/auth/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        credentials: 'include',
        body: JSON.stringify({ email, password: pw }),
      });
      const d = await res.json().catch(() => ({}));
      if (!res.ok) {
        setError(
          (d as { error?: string }).error
          ?? 'Invalid credentials.',
        );
        return;
      }
      const payload = d as LoginPayload;
      if (payload.user?.role === 'guest') {
        setError('This account does not have portal access.');
        return;
      }
      // The backend has set the nextra_sso HttpOnly cookie.
      // The main app bootstraps auth from that cookie via
      // GET /api/auth/sso-session on startup.
      window.location.href = next;
    } catch {
      setError('Network error. Please retry.');
    } finally {
      setBusy(false);
    }
  };

  return (
    <form onSubmit={submit} noValidate>
      {error && (
        <div className="error" role="alert">
          {error}
        </div>
      )}
      <label htmlFor="sso-email">Email address</label>
      <input
        id="sso-email"
        type="email"
        value={email}
        onChange={(e) => setEmail(e.target.value)}
        autoComplete="email"
        required
        suppressHydrationWarning
      />
      <label htmlFor="sso-pw">Password</label><input
        id="sso-pw"
        type="password"
        value={pw}
        onChange={(e) => setPw(e.target.value)}
        autoComplete="current-password"
        required
        suppressHydrationWarning
      />
      <button
        type="submit"
        className="btn"
        disabled={busy}
      >
        {busy ? 'Signing in…' : 'Sign in'}
      </button>
    </form>
  );
}
