'use client';

import { useCallback, useEffect, useState } from 'react';
import { useTables, useTableData } from '@/hooks';
import { DataGrid } from '@/components/molecules';
import { Spinner } from '@/components/atoms';
import labels from '@/constants/ui-labels.json';

/** @brief Panel showing table list + data. */
export default function TablesPanel() {
  const { tables, loading, fetchTables } = useTables();
  const td = useTableData();
  const [selected, setSelected] = useState('');

  useEffect(() => {
    fetchTables();
  }, [fetchTables]);

  const handleSelect = useCallback(
    (name: string) => {
      setSelected(name);
      td.fetchSchema(name);
      td.fetchData(name);
    },
    [td],
  );

  if (loading) return <Spinner />;

  return (
    <div data-testid="tables-panel">
      <h2>{labels.tables.title}</h2>
      {tables.length === 0 ? (
        <p>{labels.tables.empty}</p>
      ) : (
        <ul>
          {tables.map((t) => (
            <li key={t.table_name}>
              <button
                className={
                  selected === t.table_name
                    ? 'active'
                    : ''
                }
                onClick={
                  () => handleSelect(t.table_name)
                }
                data-testid={
                  `table-${t.table_name}`
                }
                aria-label={t.table_name}
              >
                {t.table_name}
              </button>
            </li>
          ))}
        </ul>
      )}
      {td.loading && <Spinner />}
      {td.tableData && (
        <div>
          <h3 data-testid="selected-table-name">
            {selected} ({td.tableData.rowCount}{' '}
            {labels.tables.rowCount})
          </h3>
          <DataGrid
            columns={
              td.tableData.rows[0]
                ? Object.keys(td.tableData.rows[0])
                : []
            }
            rows={td.tableData.rows}
          />
        </div>
      )}
    </div>
  );
}
