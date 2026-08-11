'use client';

import { useState } from 'react';
import type { ColDef } from '@/types/tables';
import ColumnDefRow
  from '@/components/molecules/ColumnDefRow';

/** @brief Props for CreateTableDialog. */
interface Props {
  open: boolean;
  onClose: () => void;
  onSubmit: (n: string, c: ColDef[]) => void;
}

/** @brief Dialog to create a new table. */
export default function CreateTableDialog(
  { open, onClose, onSubmit }: Props,
) {
  const [name, setName] = useState('');
  const [cols, setCols] = useState<ColDef[]>([
    { name: 'id', type: 'serial',
      primaryKey: true, notNull: true },
  ]);

  if (!open) return null;

  const addCol = () => setCols([...cols,
    { name: '', type: 'text',
      primaryKey: false, notNull: false }]);

  const updateCol = (
    i: number, field: keyof ColDef,
    val: string | boolean,
  ) => setCols(
    cols.map((c, j) =>
      j === i ? { ...c, [field]: val } : c),
  );

  return (
    <div
      className="dialog-overlay"
      data-testid="create-table-dialog"
    >
      <div
        className="dialog"
        role="dialog"
        aria-label="Create Table"
        aria-modal="true"
      >
        <h3>Create Table</h3>
        <label htmlFor="table-name">Name</label>
        <input
          id="table-name"
          value={name}
          onChange={
            (e) => setName(e.target.value)
          }
          data-testid="table-name-input"
          aria-label="Table name"
        />
        <h4>Columns</h4>
        {cols.map((c, i) => (
          <ColumnDefRow
            key={i}
            col={c}
            index={i}
            onUpdate={updateCol}
          />
        ))}
        <button
          onClick={addCol}
          aria-label="Add column"
          data-testid="add-column-btn"
        >
          + Column
        </button>
        <div className="dialog-actions">
          <button
            onClick={onClose}
            aria-label="Cancel"
            data-testid="cancel-create-table"
          >
            Cancel
          </button>
          <button
            onClick={() => onSubmit(name, cols)}
            disabled={
              !name || cols.length === 0
            }
            data-testid="submit-create-table"
            aria-label="Create"
          >
            Create
          </button>
        </div>
      </div>
    </div>
  );
}
