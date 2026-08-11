'use client';

import { useCallback } from 'react';
import { useApiCall } from './useApiCall';

/** @brief Column DDL operations. */
export function useColumnActions() {
  const { loading, error, call } =
    useApiCall<{ status: string }>();

  const addColumn = useCallback(
    async (
      tableName: string,
      columnName: string,
      dataType: string,
      notNull = false,
    ) => {
      return call('/api/pgadmin/columns', {
        method: 'POST',
        body: JSON.stringify({
          tableName, columnName, dataType, notNull,
        }),
      });
    },
    [call],
  );

  const modifyColumn = useCallback(
    async (
      tableName: string,
      columnName: string,
      newType: string,
    ) => {
      return call('/api/pgadmin/columns', {
        method: 'PUT',
        body: JSON.stringify({
          tableName, columnName, newType,
        }),
      });
    },
    [call],
  );

  const dropColumn = useCallback(
    async (
      tableName: string, columnName: string,
    ) => {
      return call('/api/pgadmin/columns', {
        method: 'DELETE',
        body: JSON.stringify({
          tableName, columnName,
        }),
      });
    },
    [call],
  );

  return {
    loading, error,
    addColumn, modifyColumn, dropColumn,
  };
}
