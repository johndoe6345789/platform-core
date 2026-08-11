'use client';
import React, { useState } from 'react';

/** Props for SsoRegisterForm. */
interface SsoRegisterFormProps {
  /** Validated redirect target after register. */
  next: string;
}

/**
 * Registration form: creates account, auto-logs in
 * (which sets the nextra_sso HttpOnly cookie), then
 * redirects.  The main app bootstraps auth from the
 * cookie via GET /api/auth/sso-session on startup.
 */
export default function SsoRegisterForm({
  next,
}: SsoRegisterFormProps) {
  const [email, setEmail] = useState('');
  const [username, setUsername] = useState('');
  const [pw, setPw] = useState('');
  const [pw2, setPw2] = useState('');
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState('');

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (pw !== pw2) {
      setError('Passwords do not match.');
      return;
    }
    setBusy(true);
    setError('');
    try {
      const reg = await fetch('/api/auth/register', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ email, username, password: pw }),
      });
      const rd = await reg.json().catch(() => ({}));
      if (!reg.ok) {
        setError((rd as { error?: string }).error ?? 'Registration failed.');
        return;
      }
      const login = await fetch('/api/auth/login', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        credentials: 'include',
        body: JSON.stringify({ email, password: pw }),
      });
      const ld = await login.json().catch(() => ({}));
      if (!login.ok) {
        window.location.href = '/sso/login';
        return;
      }
      // nextra_sso cookie set by the login response.
      void ld;
      window.location.href = next;
    } catch {
      setError('Network error. Please retry.');
    } finally {
      setBusy(false);
    }
  };

  return (
    <form onSubmit={submit} noValidate>
      {error && <div className="error" role="alert">{error}</div>}
      <label htmlFor="reg-email">Email address</label>
      <input id="reg-email" type="email" value={email}
        onChange={(e) => setEmail(e.target.value)}
        autoComplete="email" required />
      <label htmlFor="reg-username">Username</label>
      <input id="reg-username" type="text" value={username}
        onChange={(e) => setUsername(e.target.value)}
        autoComplete="username" required />
      <label htmlFor="reg-pw">Password</label>
      <input id="reg-pw" type="password" value={pw}
        onChange={(e) => setPw(e.target.value)}
        autoComplete="new-password" required />
      <label htmlFor="reg-pw2">Confirm password</label>
      <input id="reg-pw2" type="password" value={pw2}
        onChange={(e) => setPw2(e.target.value)}
        autoComplete="new-password" required />
      <button type="submit" className="btn" disabled={busy}>
        {busy ? 'Creating account…' : 'Create account'}
      </button>
      <div className="sso-links">
        <a href="/sso/login" className="link">
          Already have an account? Sign in
        </a>
      </div>
    </form>
  );
}
