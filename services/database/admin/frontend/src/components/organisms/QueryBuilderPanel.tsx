'use client';

import { useEffect } from 'react';
import { useTables } from '@/hooks/useTables';
import { useTableData } from '@/hooks/useTableData';
import { useQueryBuilder }
  from '@/hooks/useQueryBuilder';
import { TableSelector, DataGrid }
  from '@/components/molecules';
import WhereConditionRow
  from '@/components/molecules/WhereConditionRow';
import QueryOptions
  from '@/components/molecules/QueryOptions';
import { Spinner } from '@/components/atoms';

/** @brief Visual query builder panel. */
export default function QueryBuilderPanel() {
  const { tables, fetchTables } = useTables();
  const { schema, fetchSchema } = useTableData();
  const qb = useQueryBuilder();

  useEffect(() => {
    fetchTables();
  }, [fetchTables]);

  useEffect(() => {
    if (qb.table) fetchSchema(qb.table);
  }, [qb.table, fetchSchema]);

  const cols = schema?.columns
    .map((c) => c.column_name) ?? [];

  return (
    <div data-testid="query-builder-panel">
      <h2>Query Builder</h2>
      <TableSelector
        tables={tables}
        selected={qb.table}
        onSelect={qb.setTable}
      />
      {cols.length > 0 && (
        <>
          <h4>WHERE</h4>
          {qb.where.map((w, i) => (
            <WhereConditionRow
              key={i}
              condition={w}
              index={i}
              tableColumns={cols}
              onUpdate={qb.updateWhere}
              onRemove={qb.removeWhere}
            />
          ))}
          <button
            onClick={qb.addWhere}
            data-testid="add-where"
            aria-label="Add WHERE condition"
          >
            + Add Condition
          </button>
          <QueryOptions
            columns={cols}
            orderCol={qb.orderCol}
            setOrderCol={qb.setOrderCol}
            orderDir={qb.orderDir}
            setOrderDir={qb.setOrderDir}
            limit={qb.limit}
            setLimit={qb.setLimit}
            onExecute={qb.execute}
            loading={qb.loading}
            table={qb.table}
          />
        </>
      )}
      {qb.error && (
        <p className="error" role="alert"
          aria-live="assertive">
          {qb.error}
        </p>
      )}
      {qb.loading && <Spinner />}
      {qb.result && (
        <div data-testid="qb-results">
          <pre data-testid="qb-query-preview">
            {qb.result.query}
          </pre>
          <p>{qb.result.rowCount} rows</p>
          <DataGrid
            columns={
              qb.result.rows[0]
                ? Object.keys(qb.result.rows[0])
                : []
            }
            rows={qb.result.rows}
          />
        </div>
      )}
    </div>
  );
}
