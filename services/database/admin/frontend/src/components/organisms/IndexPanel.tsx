'use client';

import { useEffect, useState } from 'react';
import { useTables } from '@/hooks/useTables';
import { useTableData } from '@/hooks/useTableData';
import {
  useIndexActions,
} from '@/hooks/useIndexActions';
import { TableSelector } from '@/components/molecules';
import IndexCreateDialog
  from '@/dialogs/IndexCreateDialog';

/** @brief Panel to manage table indexes. */
export default function IndexPanel() {
  const { tables, fetchTables } = useTables();
  const { schema, fetchSchema } = useTableData();
  const ia = useIndexActions();
  const [table, setTable] = useState('');
  const [showCreate, setShowCreate] = useState(false);

  useEffect(() => { fetchTables(); }, [fetchTables]);
  useEffect(() => {
    if (table) {
      fetchSchema(table);
      ia.fetchIndexes(table);
    }
  }, [table, fetchSchema, ia.fetchIndexes]);

  const handleCreate = async (
    name: string, type: string,
    cols: string[], unique: boolean,
  ) => {
    await ia.createIndex(
      table, name, type, cols, unique);
    setShowCreate(false);
    ia.fetchIndexes(table);
  };

  const handleDrop = async (name: string) => {
    await ia.dropIndex(name);
    ia.fetchIndexes(table);
  };

  const cols = schema?.columns.map(
    (c) => c.column_name) ?? [];

  return (
    <div data-testid="index-panel">
      <h2>Index Manager</h2>
      <TableSelector tables={tables}
        selected={table} onSelect={setTable} />
      {ia.indexes.length > 0 && (
        <table>
          <thead>
            <tr>
              <th>Name</th><th>Definition</th>
              <th>Actions</th>
            </tr>
          </thead>
          <tbody>
            {ia.indexes.map((idx) => (
              <tr key={idx.indexname}>
                <td>{idx.indexname}</td>
                <td>
                  <code>{idx.indexdef}</code>
                </td>
                <td>
                  <button onClick={
                    () => handleDrop(idx.indexname)}
                    className="btn-danger"
                    aria-label={`Drop ${
                      idx.indexname}`}
                    data-testid={`drop-index-${
                      idx.indexname}`}>
                    Drop
                  </button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
      <button onClick={() => setShowCreate(true)}
        data-testid="create-index-btn"
        aria-label="Create index">
        + Create Index
      </button>
      {ia.error && (
        <p className="error"
          aria-live="assertive">
          {ia.error}
        </p>)}
      <IndexCreateDialog
        open={showCreate} tableName={table}
        columns={cols}
        onClose={() => setShowCreate(false)}
        onSubmit={handleCreate} />
    </div>
  );
}
