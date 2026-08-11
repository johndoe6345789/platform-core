'use client';
import { useCallback, useState } from 'react';

/** Shape of the enroll API response. */
interface EnrollResponse {
  secret: string;
  otpauthUri: string;
  recoveryCodes: string[];
}

/**
 * Hook wrapping the TOTP enroll + verify flow.
 */
export function useTotp() {
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState('');

  const enroll = useCallback(async () => {
    setBusy(true);
    setError('');
    try {
      const res = await fetch(
        '/api/auth/totp/enroll',
        { method: 'POST', credentials: 'include' },
      );
      if (!res.ok) {
        setError('Enrolment failed.');
        return null;
      }
      return (await res.json()) as EnrollResponse;
    } finally {
      setBusy(false);
    }
  }, []);

  const verify = useCallback(async (code: string) => {
    setBusy(true);
    setError('');
    try {
      const res = await fetch(
        '/api/auth/totp/verify',
        {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          credentials: 'include',
          body: JSON.stringify({ code }),
        },
      );
      if (!res.ok) {
        setError('Invalid code.');
        return false;
      }
      return true;
    } finally {
      setBusy(false);
    }
  }, []);

  return { busy, error, enroll, verify };
}
