'use client';
import { useCallback, useState } from 'react';

/** Result from the begin endpoint. */
interface BeginResult {
  challenge: string;
}

/**
 * Hook encapsulating the WebAuthn registration + assertion
 * network round-trips.  Keeps the UI pages minimal.
 */
export function usePasskeys() {
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState('');

  const registerBegin = useCallback(async () => {
    setBusy(true);
    try {
      const res = await fetch(
        '/api/auth/passkeys/register/begin',
        { method: 'POST', credentials: 'include' },
      );
      const d = (await res.json()) as BeginResult;
      return d;
    } finally {
      setBusy(false);
    }
  }, []);

  const registerFinish = useCallback(
    async (challenge: string, attestation: string) => {
      setBusy(true);
      try {
        const res = await fetch(
          '/api/auth/passkeys/register/finish',
          {
            method: 'POST',
            headers: {
              'Content-Type': 'application/json',
            },
            credentials: 'include',
            body: JSON.stringify({
              challenge,
              attestationObject: attestation,
            }),
          },
        );
        if (!res.ok) {
          setError('Could not register passkey.');
          return false;
        }
        return true;
      } finally {
        setBusy(false);
      }
    },
    [],
  );

  const assertBegin = useCallback(async () => {
    const res = await fetch(
      '/api/auth/passkeys/assert/begin',
      { method: 'POST' },
    );
    return (await res.json()) as BeginResult;
  }, []);

  return {
    busy,
    error,
    registerBegin,
    registerFinish,
    assertBegin,
  };
}
