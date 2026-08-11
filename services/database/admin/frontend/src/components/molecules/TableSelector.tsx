'use client';

import type { TableEntry } from '@/types/tables';

/** @brief Props for TableSelector. */
interface TableSelectorProps {
  tables: TableEntry[];
  selected: string;
  onSelect: (name: string) => void;
}

/** @brief Dropdown to pick a database table. */
export default function TableSelector(
  { tables, selected, onSelect }: TableSelectorProps,
) {
  return (
    <select
      value={selected}
      onChange={(e) => onSelect(e.target.value)}
      aria-label="Select table"
      data-testid="table-selector"
    >
      <option value="">-- Select a table --</option>
      {tables.map((t) => (
        <option key={t.table_name} value={t.table_name}>
          {t.table_name}
        </option>
      ))}
    </select>
  );
}
