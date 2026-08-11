'use client';

import { useEffect, useState } from 'react';
import { useTables } from '@/hooks/useTables';
import { useTableData } from '@/hooks/useTableData';
import {
  useConstraintActions,
} from '@/hooks/useConstraintActions';
import { TableSelector } from '@/components/molecules';
import ConstraintDialog
  from '@/dialogs/ConstraintDialog';

/** @brief Panel to manage table constraints. */
export default function ConstraintPanel() {
  const { tables, fetchTables } = useTables();
  const { schema, fetchSchema } = useTableData();
  const ca = useConstraintActions();
  const [table, setTable] = useState('');
  const [showAdd, setShowAdd] = useState(false);

  useEffect(() => { fetchTables(); }, [fetchTables]);
  useEffect(() => {
    if (table) {
      fetchSchema(table);
      ca.fetchConstraints(table);
    }
  }, [table, fetchSchema, ca.fetchConstraints]);

  const handleAdd = async (
    type: string, col: string, expr?: string,
  ) => {
    await ca.addConstraint(table, type, col, expr);
    setShowAdd(false);
    ca.fetchConstraints(table);
  };

  const handleDrop = async (name: string) => {
    await ca.dropConstraint(table, name);
    ca.fetchConstraints(table);
  };

  const cols = schema?.columns.map(
    (c) => c.column_name) ?? [];

  return (
    <div data-testid="constraint-panel">
      <h2>Constraint Manager</h2>
      <TableSelector tables={tables}
        selected={table} onSelect={setTable} />
      {ca.constraints.length > 0 && (
        <table>
          <thead>
            <tr>
              <th>Name</th><th>Type</th>
              <th>Column</th><th>Actions</th>
            </tr>
          </thead>
          <tbody>
            {ca.constraints.map((c) => (
              <tr key={c.constraint_name}>
                <td>{c.constraint_name}</td>
                <td>{c.constraint_type}</td>
                <td>{c.column_name}</td>
                <td>
                  <button onClick={
                    () => handleDrop(
                      c.constraint_name)}
                    className="btn-danger"
                    aria-label={`Drop ${
                      c.constraint_name}`}
                    data-testid={`drop-constraint-${
                      c.constraint_name}`}>
                    Drop
                  </button>
                </td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
      <button onClick={() => setShowAdd(true)}
        data-testid="add-constraint-btn"
        aria-label="Add constraint">
        + Add Constraint
      </button>
      {ca.error && (
        <p className="error"
          aria-live="assertive">
          {ca.error}
        </p>)}
      <ConstraintDialog
        open={showAdd} tableName={table}
        columns={cols}
        onClose={() => setShowAdd(false)}
        onSubmit={handleAdd} />
    </div>
  );
}
