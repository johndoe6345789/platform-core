'use client';

import { useState } from 'react';

/** @brief Props for IndexCreateDialog. */
interface Props {
  open: boolean;
  tableName: string;
  columns: string[];
  onClose: () => void;
  onSubmit: (
    name: string, type: string,
    cols: string[], unique: boolean,
  ) => void;
}

const INDEX_TYPES = [
  'BTREE', 'HASH', 'GIN', 'GIST', 'BRIN',
];

/** @brief Dialog to create an index. */
export default function IndexCreateDialog(
  {
    open, tableName, columns, onClose, onSubmit,
  }: Props,
) {
  const [name, setName] = useState('');
  const [type, setType] = useState('BTREE');
  const [selCols, setSelCols] =
    useState<string[]>([]);
  const [unique, setUnique] = useState(false);

  if (!open) return null;

  const toggleCol = (c: string) =>
    setSelCols((prev) =>
      prev.includes(c)
        ? prev.filter((x) => x !== c)
        : [...prev, c],
    );

  return (
    <div
      className="dialog-overlay"
      data-testid="index-create-dialog"
    >
      <div className="dialog" role="dialog"
        aria-label="Create Index"
        aria-modal="true">
        <h3>Create Index on {tableName}</h3>
        <label htmlFor="idx-name">Name</label>
        <input id="idx-name" value={name}
          onChange={(e) => setName(e.target.value)}
          data-testid="idx-name-input"
          aria-label="Index name" />
        <label htmlFor="idx-type">Type</label>
        <select id="idx-type" value={type}
          onChange={(e) => setType(e.target.value)}
          aria-label="Index type">
          {INDEX_TYPES.map((t) => (
            <option key={t} value={t}>{t}</option>
          ))}
        </select>
        <label>
          <input type="checkbox" checked={unique}
            data-testid="unique-checkbox"
            onChange={
              (e) => setUnique(e.target.checked)
            } /> Unique
        </label>
        <h4>Columns</h4>
        {columns.map((c) => (
          <label key={c}>
            <input type="checkbox"
              checked={selCols.includes(c)}
              onChange={() => toggleCol(c)} />
            {c}
          </label>
        ))}
        <div className="dialog-actions">
          <button onClick={onClose}
            aria-label="Cancel"
            data-testid="cancel-create-index">
            Cancel
          </button>
          <button
            onClick={() => onSubmit(
              name, type, selCols, unique)}
            disabled={
              !name || selCols.length === 0
            }
            data-testid="submit-create-index"
            aria-label="Create index">
            Create
          </button>
        </div>
      </div>
    </div>
  );
}
