'use client';

import { useCallback, useState } from 'react';
import { useApiCall } from './useApiCall';
import type {
  ConstraintEntry,
} from '@/types/constraints';

/** @brief Constraint list response. */
interface ConstraintsRes {
  constraints: ConstraintEntry[];
}

/** @brief Constraint management operations. */
export function useConstraintActions() {
  const [constraints, setConstraints] =
    useState<ConstraintEntry[]>([]);
  const listApi = useApiCall<ConstraintsRes>();
  const mutApi =
    useApiCall<{ status: string }>();

  const fetchConstraints = useCallback(
    async (table: string) => {
      const data = await listApi.call(
        `/api/pgadmin/constraints?table=${table}`,
      );
      if (data) setConstraints(data.constraints);
    },
    [listApi.call],
  );

  const addConstraint = useCallback(
    async (
      tableName: string,
      type: string,
      columnName: string,
      expression?: string,
    ) => {
      return mutApi.call(
        '/api/pgadmin/constraints',
        {
          method: 'POST',
          body: JSON.stringify({
            tableName, type, columnName, expression,
          }),
        },
      );
    },
    [mutApi.call],
  );

  const dropConstraint = useCallback(
    async (
      tableName: string,
      constraintName: string,
    ) => {
      return mutApi.call(
        '/api/pgadmin/constraints',
        {
          method: 'DELETE',
          body: JSON.stringify({
            tableName, constraintName,
          }),
        },
      );
    },
    [mutApi.call],
  );

  return {
    constraints,
    loading: listApi.loading || mutApi.loading,
    error: listApi.error || mutApi.error,
    fetchConstraints,
    addConstraint,
    dropConstraint,
  };
}
