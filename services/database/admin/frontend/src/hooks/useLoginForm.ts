'use client';

import { useState } from 'react';
import { useRouter } from 'next/navigation';
import type { LoginResponse } from '@/types/auth';

/** @brief Return type for the useLoginForm hook. */
export interface UseLoginFormResult {
  username: string;
  setUsername: (v: string) => void;
  password: string;
  setPassword: (v: string) => void;
  error: string;
  loading: boolean;
  handleSubmit: (e: React.FormEvent) => void;
}

/**
 * @brief Hook encapsulating login form state
 * and submission logic.
 * @param onLogin - callback on successful login.
 * @returns Form state and handlers.
 */
export function useLoginForm(
  onLogin: (token: string, user: string) => void,
): UseLoginFormResult {
  const router = useRouter();
  const [username, setUsername] = useState('');
  const [password, setPassword] = useState('');
  const [error, setError] = useState('');
  const [loading, setLoading] = useState(false);

  const handleSubmit = async (
    e: React.FormEvent,
  ) => {
    e.preventDefault();
    setError('');
    setLoading(true);
    try {
      const res = await fetch(
        '/api/pgadmin/auth/login',
        {
          method: 'POST',
          headers: {
            'Content-Type': 'application/json',
          },
          body: JSON.stringify(
            { username, password },
          ),
        },
      );
      const data: LoginResponse =
        await res.json();
      if (!res.ok) {
        setError('Invalid credentials');
        setLoading(false);
        return;
      }
      onLogin(data.token, data.username);
      router.push('/dashboard');
    } catch {
      setError('Connection error');
      setLoading(false);
    }
  };

  return {
    username, setUsername,
    password, setPassword,
    error, loading, handleSubmit,
  };
}
