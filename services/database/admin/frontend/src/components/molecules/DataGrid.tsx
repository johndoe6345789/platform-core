'use client';

import styles
  from '@scss/atoms/pgadmin-shell.module.scss';

/** @brief Props for DataGrid. */
interface DataGridProps {
  columns: string[];
  rows: Record<string, string | null>[];
}

/** @brief Scrollable data table with headers. */
export default function DataGrid(
  { columns, rows }: DataGridProps,
) {
  if (columns.length === 0) {
    return (
      <p data-testid="datagrid-empty">
        No data to display
      </p>
    );
  }

  return (
    <div
      className={styles.tableWrapper}
      data-testid="datagrid"
    >
      <table
        className={styles.dataTable}
        data-testid="datagrid-table"
        aria-label="Data table"
      >
        <thead>
          <tr>
            {columns.map((col) => (
              <th key={col}>{col}</th>
            ))}
          </tr>
        </thead>
        <tbody>
          {rows.map((row, i) => (
            <tr key={i}>
              {columns.map((col) => (
                <td key={col}>
                  {row[col] ?? <em>NULL</em>}
                </td>
              ))}
            </tr>
          ))}
        </tbody>
      </table>
    </div>
  );
}
