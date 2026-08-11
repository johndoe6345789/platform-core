'use client';

import { useCallback, useState } from 'react';
import { useApiCall } from './useApiCall';
import type {
  QueryBuilderParams,
  QueryResult,
  WhereCondition,
} from '@/types/queries';

/** @brief Visual query builder state + execution. */
export function useQueryBuilder() {
  const [table, setTable] = useState('');
  const [columns, setColumns] =
    useState<string[]>([]);
  const [where, setWhere] =
    useState<WhereCondition[]>([]);
  const [orderCol, setOrderCol] = useState('');
  const [orderDir, setOrderDir] =
    useState<'ASC' | 'DESC'>('ASC');
  const [limit, setLimit] = useState(100);
  const [result, setResult] =
    useState<QueryResult | null>(null);
  const { loading, error, call } =
    useApiCall<QueryResult>();

  const addWhere = useCallback(() => {
    setWhere((prev) => [
      ...prev,
      { column: '', operator: '=', value: '' },
    ]);
  }, []);

  const removeWhere = useCallback((idx: number) => {
    setWhere((prev) =>
      prev.filter((_, i) => i !== idx),
    );
  }, []);

  const updateWhere = useCallback(
    (idx: number, field: keyof WhereCondition,
     val: string) => {
      setWhere((prev) =>
        prev.map((w, i) =>
          i === idx ? { ...w, [field]: val } : w,
        ),
      );
    },
    [],
  );

  const execute = useCallback(async () => {
    const params: QueryBuilderParams = {
      table,
      columns: columns.length > 0
        ? columns
        : undefined,
      where: where.length > 0
        ? where
        : undefined,
      orderBy: orderCol
        ? { column: orderCol, direction: orderDir }
        : undefined,
      limit,
    };
    const data = await call(
      '/api/pgadmin/query/build',
      {
        method: 'POST',
        body: JSON.stringify(params),
      },
    );
    if (data) setResult(data);
  }, [
    table, columns, where, orderCol,
    orderDir, limit, call,
  ]);

  return {
    table, setTable,
    columns, setColumns,
    where, addWhere, removeWhere, updateWhere,
    orderCol, setOrderCol,
    orderDir, setOrderDir,
    limit, setLimit,
    result, loading, error, execute,
  };
}
