'use client';

import { useState } from 'react';

/** @brief Props for ConstraintDialog. */
interface Props {
  open: boolean;
  tableName: string;
  columns: string[];
  onClose: () => void;
  onSubmit: (
    type: string, column: string,
    expression?: string,
  ) => void;
}

const TYPES = ['UNIQUE', 'NOT NULL', 'CHECK'];

/** @brief Dialog to add a constraint. */
export default function ConstraintDialog(
  {
    open, tableName, columns, onClose, onSubmit,
  }: Props,
) {
  const [type, setType] = useState('UNIQUE');
  const [col, setCol] = useState('');
  const [expr, setExpr] = useState('');

  if (!open) return null;

  return (
    <div
      className="dialog-overlay"
      data-testid="constraint-dialog"
    >
      <div className="dialog" role="dialog"
        aria-label="Add Constraint"
        aria-modal="true">
        <h3>Add Constraint to {tableName}</h3>
        <label htmlFor="c-type">Type</label>
        <select id="c-type" value={type}
          onChange={(e) => setType(e.target.value)}
          aria-label="Constraint type">
          {TYPES.map((t) => (
            <option key={t} value={t}>{t}</option>
          ))}
        </select>
        <label htmlFor="c-col">Column</label>
        <select id="c-col" value={col}
          onChange={(e) => setCol(e.target.value)}
          aria-label="Column">
          <option value="">Select</option>
          {columns.map((c) => (
            <option key={c} value={c}>{c}</option>
          ))}
        </select>
        {type === 'CHECK' && (
          <>
            <label htmlFor="c-expr">Expression</label>
            <input id="c-expr" value={expr}
              onChange={
                (e) => setExpr(e.target.value)
              }
              placeholder="column > 0"
              aria-label="Check expression" />
          </>
        )}
        <div className="dialog-actions">
          <button onClick={onClose}
            aria-label="Cancel"
            data-testid="cancel-constraint">
            Cancel
          </button>
          <button
            onClick={() => onSubmit(
              type, col,
              type === 'CHECK' ? expr : undefined)}
            disabled={!col}
            data-testid="submit-constraint"
            aria-label="Add constraint">
            Add
          </button>
        </div>
      </div>
    </div>
  );
}
