'use client';
import React, { useState } from 'react';
import { useTotp } from '@/hooks/useTotp';

/** Shape of a completed enrolment. */
interface Enrolment {
  secret: string;
  otpauthUri: string;
  recoveryCodes: string[];
}

/**
 * TOTP 2FA enrolment page.  Step 1: user clicks "Start",
 * server returns a secret + otpauth URI + recovery codes.
 * Step 2: user scans the QR, enters a 6-digit code, server
 * flips `enabled` on success.
 */
export default function TotpPage() {
  const { busy, error, enroll, verify } = useTotp();
  const [data, setData] = useState<Enrolment | null>(null);
  const [code, setCode] = useState('');
  const [done, setDone] = useState(false);

  const start = async () => {
    const d = await enroll();
    if (d) setData(d);
  };

  const onVerify = async (e: React.FormEvent) => {
    e.preventDefault();
    const ok = await verify(code);
    if (ok) setDone(true);
  };

  return (
    <div className="card" data-testid="totp-page">
      <p className="logo">NextExtra</p>
      <p className="subtitle">Set up 2FA</p>
      {error && (
        <div className="error" role="alert">
          {error}
        </div>
      )}
      {!data && (
        <button
          type="button"
          className="btn"
          disabled={busy}
          onClick={start}
          aria-label="Start TOTP enrolment"
        >
          {busy ? 'Working...' : 'Begin setup'}
        </button>
      )}
      {data && !done && (
        <form onSubmit={onVerify}>
          <p>Scan this URI in your authenticator app:</p>
          <code data-testid="totp-uri">
            {data.otpauthUri}
          </code>
          <label htmlFor="totp-code">Enter code</label>
          <input
            id="totp-code"
            value={code}
            onChange={(e) => setCode(e.target.value)}
            inputMode="numeric"
            required
          />
          <button className="btn" disabled={busy}>
            Verify
          </button>
        </form>
      )}
      {done && (
        <p data-testid="totp-done">2FA enabled.</p>
      )}
    </div>
  );
}
