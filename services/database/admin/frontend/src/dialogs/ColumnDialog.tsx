'use client';

import { useState } from 'react';
import dataTypes from '@/constants/data-types.json';

/** @brief Props for ColumnDialog. */
interface Props {
  open: boolean;
  tableName: string;
  onClose: () => void;
  onSubmit: (
    col: string, type: string, notNull: boolean,
  ) => void;
}

/** @brief Dialog to add a column. */
export default function ColumnDialog(
  { open, tableName, onClose, onSubmit }: Props,
) {
  const [col, setCol] = useState('');
  const [type, setType] = useState('text');
  const [notNull, setNotNull] = useState(false);

  if (!open) return null;

  return (
    <div
      className="dialog-overlay"
      data-testid="column-dialog"
    >
      <div className="dialog" role="dialog"
        aria-label="Add Column"
        aria-modal="true">
        <h3>Add Column to {tableName}</h3>
        <label htmlFor="col-name">Name</label>
        <input id="col-name" value={col}
          onChange={(e) => setCol(e.target.value)}
          aria-label="Column name"
          data-testid="col-name-input" />
        <label htmlFor="col-type">Type</label>
        <select id="col-type" value={type}
          onChange={(e) => setType(e.target.value)}
          aria-label="Column type">
          {dataTypes.map((t) => (
            <option key={t} value={t}>{t}</option>
          ))}
        </select>
        <label>
          <input type="checkbox" checked={notNull}
            data-testid="col-notnull-check"
            onChange={
              (e) => setNotNull(e.target.checked)
            } /> NOT NULL
        </label>
        <div className="dialog-actions">
          <button onClick={onClose}
            aria-label="Cancel"
            data-testid="cancel-add-column">
            Cancel
          </button>
          <button
            onClick={() => onSubmit(
              col, type, notNull)}
            disabled={!col}
            data-testid="submit-add-column"
            aria-label="Add column">
            Add
          </button>
        </div>
      </div>
    </div>
  );
}
