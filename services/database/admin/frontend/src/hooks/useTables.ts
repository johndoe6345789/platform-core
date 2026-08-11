'use client';

import { useCallback, useState } from 'react';
import { useApiCall } from './useApiCall';
import type { TableEntry } from '@/types/tables';

/** @brief Tables response shape. */
interface TablesRes {
  tables: TableEntry[];
}

/** @brief Fetch and manage the table list. */
export function useTables() {
  const [tables, setTables] =
    useState<TableEntry[]>([]);
  const { loading, error, call } =
    useApiCall<TablesRes>();

  const fetchTables = useCallback(async () => {
    const data = await call('/api/pgadmin/tables');
    if (data) setTables(data.tables);
  }, [call]);

  return { tables, loading, error, fetchTables };
}
