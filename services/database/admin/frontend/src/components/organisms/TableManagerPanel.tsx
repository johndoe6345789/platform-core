'use client';

import { useEffect, useState } from 'react';
import { useTables } from '@/hooks/useTables';
import { useApiCall } from '@/hooks/useApiCall';
import CreateTableDialog
  from '@/dialogs/CreateTableDialog';
import DropTableDialog
  from '@/dialogs/DropTableDialog';

/** @brief Panel to create and drop tables. */
export default function TableManagerPanel() {
  const { tables, fetchTables } = useTables();
  const { call } =
    useApiCall<{ status: string }>();
  const [showCreate, setShowCreate] = useState(false);
  const [dropTarget, setDropTarget] = useState('');

  useEffect(() => { fetchTables(); }, [fetchTables]);

  const handleCreate = async (
    name: string,
    columns: Array<{
      name: string; type: string;
      primaryKey: boolean; notNull: boolean;
    }>,
  ) => {
    await call('/api/pgadmin/tables/manage', {
      method: 'POST',
      body: JSON.stringify({
        tableName: name, columns,
      }),
    });
    setShowCreate(false);
    fetchTables();
  };

  const handleDrop = async () => {
    await call('/api/pgadmin/tables/manage', {
      method: 'DELETE',
      body: JSON.stringify({
        tableName: dropTarget,
      }),
    });
    setDropTarget('');
    fetchTables();
  };

  return (
    <div data-testid="table-manager-panel">
      <h2>Table Manager</h2>
      <button onClick={() => setShowCreate(true)}
        data-testid="create-table-btn"
        aria-label="Create table">
        + Create Table
      </button>
      <ul>
        {tables.map((t) => (
          <li key={t.table_name}>
            {t.table_name}
            <button
              onClick={
                () => setDropTarget(t.table_name)
              }
              className="btn-danger"
              aria-label={
                `Drop ${t.table_name}`
              }
              data-testid={`drop-table-${
                t.table_name}`}
            >
              Drop
            </button>
          </li>
        ))}
      </ul>
      <CreateTableDialog
        open={showCreate}
        onClose={() => setShowCreate(false)}
        onSubmit={handleCreate}
      />
      <DropTableDialog
        open={!!dropTarget}
        tableName={dropTarget}
        onConfirm={handleDrop}
        onCancel={() => setDropTarget('')}
      />
    </div>
  );
}
