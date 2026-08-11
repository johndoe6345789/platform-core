'use client';

import { useEffect, useState } from 'react';
import { useTables } from '@/hooks/useTables';
import { useTableData } from '@/hooks/useTableData';
import {
  useColumnActions,
} from '@/hooks/useColumnActions';
import { TableSelector } from '@/components/molecules';
import ColumnDialog from '@/dialogs/ColumnDialog';

/** @brief Panel to add/modify/drop columns. */
export default function ColumnManagerPanel() {
  const { tables, fetchTables } = useTables();
  const { schema, fetchSchema } = useTableData();
  const { addColumn, dropColumn, loading, error } =
    useColumnActions();
  const [table, setTable] = useState('');
  const [showAdd, setShowAdd] = useState(false);

  useEffect(() => { fetchTables(); }, [fetchTables]);
  useEffect(() => {
    if (table) fetchSchema(table);
  }, [table, fetchSchema]);

  const handleAdd = async (
    col: string, type: string, notNull: boolean,
  ) => {
    await addColumn(table, col, type, notNull);
    setShowAdd(false);
    fetchSchema(table);
  };

  const handleDrop = async (col: string) => {
    await dropColumn(table, col);
    fetchSchema(table);
  };

  return (
    <div data-testid="column-manager-panel">
      <h2>Column Manager</h2>
      <TableSelector
        tables={tables} selected={table}
        onSelect={setTable} />
      {schema && (
        <>
          <button onClick={() => setShowAdd(true)}
            data-testid="add-col-btn"
            aria-label="Add column">
            + Add Column
          </button>
          <table>
            <thead>
              <tr>
                <th>Name</th><th>Type</th>
                <th>Nullable</th><th>Actions</th>
              </tr>
            </thead>
            <tbody>
              {schema.columns.map((c) => (
                <tr key={c.column_name}>
                  <td>{c.column_name}</td>
                  <td>{c.data_type}</td>
                  <td>{c.is_nullable}</td>
                  <td>
                    <button
                      onClick={
                        () => handleDrop(
                          c.column_name)
                      }
                      className="btn-danger"
                      aria-label={
                        `Drop ${c.column_name}`
                      }
                      data-testid={`drop-col-${
                        c.column_name}`}
                    >
                      Drop
                    </button>
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </>
      )}
      {error && (
        <p className="error" aria-live="assertive">
          {error}
        </p>
      )}
      <ColumnDialog
        open={showAdd} tableName={table}
        onClose={() => setShowAdd(false)}
        onSubmit={handleAdd} />
    </div>
  );
}
