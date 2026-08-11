'use client';

import { useCallback, useState } from 'react';
import { useApiCall } from './useApiCall';
import type { QueryResult } from '@/types/queries';

/** @brief Execute raw SQL queries. */
export function useQueryExec() {
  const [result, setResult] =
    useState<QueryResult | null>(null);
  const { loading, error, call } =
    useApiCall<QueryResult>();

  const execute = useCallback(
    async (query: string) => {
      const data = await call('/api/pgadmin/query', {
        method: 'POST',
        body: JSON.stringify({ query }),
      });
      if (data) setResult(data);
      return data;
    },
    [call],
  );

  return { result, loading, error, execute };
}
