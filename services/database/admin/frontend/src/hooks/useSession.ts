'use client';

import { useCallback, useEffect, useState } from 'react';
import type { SessionState } from '@/types/auth';

/** @brief Manage client-side session state. */
export function useSession() {
  const [session, setSession] =
    useState<SessionState>({
      token: null,
      username: null,
      isAuthenticated: false,
    });
  const [ready, setReady] = useState(false);

  useEffect(() => {
    const token =
      localStorage.getItem('pgadmin-token');
    const username =
      localStorage.getItem('pgadmin-user');
    if (token && username) {
      setSession({
        token,
        username,
        isAuthenticated: true,
      });
    }
    setReady(true);
  }, []);

  const login = useCallback(
    (token: string, username: string) => {
      localStorage.setItem('pgadmin-token', token);
      localStorage.setItem('pgadmin-user', username);
      setSession({
        token,
        username,
        isAuthenticated: true,
      });
    },
    [],
  );

  const logout = useCallback(() => {
    localStorage.removeItem('pgadmin-token');
    localStorage.removeItem('pgadmin-user');
    setSession({
      token: null,
      username: null,
      isAuthenticated: false,
    });
  }, []);

  return { session, ready, login, logout };
}
