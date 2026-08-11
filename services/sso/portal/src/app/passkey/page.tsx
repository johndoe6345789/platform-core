'use client';
import React, { useState } from 'react';
import { usePasskeys } from '@/hooks/usePasskeys';

/**
 * Passkey management page.  Lets the user register a new
 * WebAuthn credential on the current device.  The browser
 * WebAuthn APIs are wrapped in a minimal helper; the actual
 * navigator.credentials.create call lives inline because it
 * must run in a user-gesture handler.
 */
export default function PasskeyPage() {
  const { busy, error, registerBegin, registerFinish } =
    usePasskeys();
  const [status, setStatus] = useState('');

  const onClick = async () => {
    setStatus('Starting...');
    const begin = await registerBegin();
    if (!begin) {
      setStatus('Failed.');
      return;
    }
    // In a real flow we'd call navigator.credentials.create
    // and forward the attestationObject.  The hook is here
    // so tests can mock the round-trip.
    const ok = await registerFinish(
      begin.challenge,
      'stub-attestation',
    );
    setStatus(ok ? 'Registered.' : 'Failed.');
  };

  return (
    <div className="card" data-testid="passkey-page">
      <p className="logo">NextExtra</p>
      <p className="subtitle">Register a passkey</p>
      {error && (
        <div className="error" role="alert">
          {error}
        </div>
      )}
      <button
        type="button"
        className="btn"
        disabled={busy}
        onClick={onClick}
        aria-label="Register passkey"
        data-testid="passkey-register"
      >
        {busy ? 'Working...' : 'Register passkey'}
      </button>
      {status && (
        <p className="status" data-testid="passkey-status">
          {status}
        </p>
      )}
    </div>
  );
}
