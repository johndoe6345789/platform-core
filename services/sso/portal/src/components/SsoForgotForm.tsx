'use client';
import React, { useState } from 'react';

/** Props for SsoForgotForm. */
interface SsoForgotFormProps {
  /** Where to return after password is reset. */
  next: string;
}

/**
 * Forgot-password form: POSTs email to
 * /api/auth/forgot-password and shows a
 * confirmation message on success.
 */
export default function SsoForgotForm({
  next,
}: SsoForgotFormProps) {
  const [email, setEmail] = useState('');
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState('');
  const [sent, setSent] = useState(false);

  const submit = async (e: React.FormEvent) => {
    e.preventDefault();
    setBusy(true);
    setError('');
    try {
      const res = await fetch(
        '/api/auth/forgot-password',
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify({ email }),
        },
      );
      if (!res.ok) {
        const d = await res.json().catch(
          () => ({}),
        );
        setError(
          (d as { error?: string }).error
          ?? 'Request failed.',
        );
        return;
      }
      setSent(true);
    } catch {
      setError('Network error. Please retry.');
    } finally {
      setBusy(false);
    }
  };

  const loginHref = `/sso/login${next !== '/'
    ? `?next=${encodeURIComponent(next)}` : ''}`;

  if (sent) {
    return (
      <div>
        <p className="subtitle" style={{ marginBottom: 0 }}>
          Check your email for a reset link.
        </p>
        <div className="sso-links" style={{ marginTop: 24 }}>
          <a href={loginHref} className="link">
            Back to sign in
          </a>
        </div>
      </div>
    );
  }

  return (
    <form onSubmit={submit} noValidate>
      {error && (
        <div className="error" role="alert">{error}</div>
      )}
      <label htmlFor="forgot-email">Email address</label>
      <input
        id="forgot-email"
        type="email"
        value={email}
        onChange={(e) => setEmail(e.target.value)}
        autoComplete="email"
        required
      />
      <button
        type="submit"
        className="btn"
        disabled={busy}
      >
        {busy ? 'Sending…' : 'Send reset link'}
      </button>
      <div className="sso-links" style={{ marginTop: 16 }}>
        <a href={loginHref} className="link">Back to sign in</a>
      </div>
    </form>
  );
}
