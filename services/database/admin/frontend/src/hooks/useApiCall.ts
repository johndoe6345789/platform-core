'use client';

import { useCallback, useState } from 'react';

/** @brief State returned by useApiCall. */
interface ApiCallState<T> {
  data: T | null;
  loading: boolean;
  error: string | null;
}

/** @brief Generic fetch wrapper with auth. */
export function useApiCall<T>() {
  const [state, setState] = useState<ApiCallState<T>>(
    { data: null, loading: false, error: null },
  );

  const call = useCallback(
    async (url: string, options?: RequestInit) => {
      setState(
        { data: null, loading: true, error: null },
      );
      try {
        const token =
          localStorage.getItem('pgadmin-token');
        const headers: Record<string, string> = {
          'Content-Type': 'application/json',
          ...(token
            ? { Authorization: `Bearer ${token}` }
            : {}),
        };
        const res = await fetch(url, {
          ...options,
          headers: { ...headers, ...options?.headers },
        });
        if (res.status === 401) {
          localStorage.removeItem('pgadmin-token');
          window.location.href = '/login';
          return null;
        }
        if (!res.ok) {
          const err = await res.json();
          throw new Error(
            err.error || `HTTP ${res.status}`,
          );
        }
        const data = (await res.json()) as T;
        setState({ data, loading: false, error: null });
        return data;
      } catch (e) {
        const msg =
          e instanceof Error ? e.message : 'Error';
        setState(
          { data: null, loading: false, error: msg },
        );
        return null;
      }
    },
    [],
  );

  return { ...state, call };
}
