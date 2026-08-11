'use client';

import { useCallback, useState } from 'react';
import { useApiCall } from './useApiCall';
import type {
  TableData,
  TableSchema,
} from '@/types/tables';

/** @brief Fetch table schema and row data. */
export function useTableData() {
  const [schema, setSchema] =
    useState<TableSchema | null>(null);
  const [tableData, setTableData] =
    useState<TableData | null>(null);
  const schemaApi = useApiCall<TableSchema>();
  const dataApi = useApiCall<TableData>();

  const fetchSchema = useCallback(
    async (tableName: string) => {
      const res = await schemaApi.call(
        '/api/pgadmin/tables/schema',
        {
          method: 'POST',
          body: JSON.stringify({ tableName }),
        },
      );
      if (res) setSchema(res);
    },
    [schemaApi.call],
  );

  const fetchData = useCallback(
    async (tableName: string) => {
      const res = await dataApi.call(
        '/api/pgadmin/tables/data',
        {
          method: 'POST',
          body: JSON.stringify({ tableName }),
        },
      );
      if (res) setTableData(res);
    },
    [dataApi.call],
  );

  return {
    schema,
    tableData,
    loading:
      schemaApi.loading || dataApi.loading,
    error: schemaApi.error || dataApi.error,
    fetchSchema,
    fetchData,
  };
}
