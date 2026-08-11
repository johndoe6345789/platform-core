'use client';

import { useCallback, useState } from 'react';
import { useApiCall } from './useApiCall';
import type { IndexEntry } from '@/types/indexes';

/** @brief Index list response. */
interface IndexesRes {
  indexes: IndexEntry[];
}

/** @brief Index management operations. */
export function useIndexActions() {
  const [indexes, setIndexes] =
    useState<IndexEntry[]>([]);
  const listApi = useApiCall<IndexesRes>();
  const mutApi =
    useApiCall<{ status: string }>();

  const fetchIndexes = useCallback(
    async (table: string) => {
      const data = await listApi.call(
        `/api/pgadmin/indexes?table=${table}`,
      );
      if (data) setIndexes(data.indexes);
    },
    [listApi.call],
  );

  const createIndex = useCallback(
    async (
      tableName: string,
      indexName: string,
      indexType: string,
      columns: string[],
      unique: boolean,
    ) => {
      return mutApi.call('/api/pgadmin/indexes', {
        method: 'POST',
        body: JSON.stringify({
          tableName, indexName, indexType,
          columns, unique,
        }),
      });
    },
    [mutApi.call],
  );

  const dropIndex = useCallback(
    async (indexName: string) => {
      return mutApi.call('/api/pgadmin/indexes', {
        method: 'DELETE',
        body: JSON.stringify({ indexName }),
      });
    },
    [mutApi.call],
  );

  return {
    indexes,
    loading: listApi.loading || mutApi.loading,
    error: listApi.error || mutApi.error,
    fetchIndexes, createIndex, dropIndex,
  };
}
