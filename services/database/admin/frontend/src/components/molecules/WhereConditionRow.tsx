'use client';

import type { WhereCondition } from '@/types/queries';

/** @brief Props for WhereConditionRow. */
interface WhereConditionRowProps {
  condition: WhereCondition;
  index: number;
  tableColumns: string[];
  onUpdate: (
    idx: number,
    field: keyof WhereCondition,
    value: string,
  ) => void;
  onRemove: (idx: number) => void;
}

const OPERATORS = [
  '=', '!=', '>', '<', '>=', '<=',
  'LIKE', 'IS NULL', 'IS NOT NULL',
];

/** @brief Single WHERE condition editor row. */
export default function WhereConditionRow(
  {
    condition, index, tableColumns,
    onUpdate, onRemove,
  }: WhereConditionRowProps,
) {
  return (
    <div
      className="where-row"
      data-testid={`where-row-${index}`}
    >
      <select
        value={condition.column}
        onChange={
          (e) => onUpdate(
            index, 'column', e.target.value,
          )
        }
        aria-label="Column"
        data-testid={`where-col-${index}`}
      >
        <option value="">Column</option>
        {tableColumns.map((c) => (
          <option key={c} value={c}>{c}</option>
        ))}
      </select>
      <select
        value={condition.operator}
        onChange={
          (e) => onUpdate(
            index, 'operator', e.target.value,
          )
        }
        aria-label="Operator"
        data-testid={`where-op-${index}`}
      >
        {OPERATORS.map((op) => (
          <option key={op} value={op}>{op}</option>
        ))}
      </select>
      {!['IS NULL', 'IS NOT NULL'].includes(
        condition.operator,
      ) && (
        <input
          type="text"
          value={condition.value}
          onChange={
            (e) => onUpdate(
              index, 'value', e.target.value,
            )
          }
          placeholder="Value"
          aria-label="Value"
          data-testid={`where-val-${index}`}
        />
      )}
      <button
        onClick={() => onRemove(index)}
        aria-label="Remove condition"
        data-testid={`where-remove-${index}`}
      >
        ×
      </button>
    </div>
  );
}
