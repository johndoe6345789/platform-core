import type { ColDef } from '@/types/tables';
import dataTypes from '@/constants/data-types.json';

/** @brief Props for ColumnDefRow molecule. */
interface ColumnDefRowProps {
  col: ColDef;
  index: number;
  onUpdate: (
    i: number,
    field: keyof ColDef,
    val: string | boolean,
  ) => void;
}

/**
 * @brief A single editable column definition
 * row used in the create-table dialog.
 */
export default function ColumnDefRow({
  col, index, onUpdate,
}: ColumnDefRowProps) {
  return (
    <div
      className="col-row"
      data-testid={`col-row-${index}`}
    >
      <input
        value={col.name}
        placeholder="Name"
        onChange={(e) =>
          onUpdate(
            index, 'name', e.target.value,
          )}
        aria-label="Column name"
        data-testid={`col-name-${index}`}
      />
      <select
        value={col.type}
        onChange={(e) =>
          onUpdate(
            index, 'type', e.target.value,
          )}
        aria-label="Column type"
        data-testid={`col-type-${index}`}
      >
        {dataTypes.map((t) => (
          <option key={t} value={t}>{t}</option>
        ))}
      </select>
      <label>
        <input
          type="checkbox"
          checked={col.primaryKey}
          onChange={(e) =>
            onUpdate(
              index, 'primaryKey',
              e.target.checked,
            )}
          aria-label="Primary key"
          data-testid={`col-pk-${index}`}
        /> PK
      </label>
      <label>
        <input
          type="checkbox"
          checked={col.notNull}
          onChange={(e) =>
            onUpdate(
              index, 'notNull',
              e.target.checked,
            )}
          aria-label="Not null"
          data-testid={`col-notnull-${index}`}
        /> NOT NULL
      </label>
    </div>
  );
}
