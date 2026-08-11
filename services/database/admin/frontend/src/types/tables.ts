/** @brief Table-related types. */

/** @brief A table entry from the list endpoint. */
export interface TableEntry {
  table_name: string;
}

/** @brief Column info from information_schema. */
export interface ColumnInfo {
  column_name: string;
  data_type: string;
  character_maximum_length: number | null;
  is_nullable: string;
  column_default: string | null;
}

/** @brief Table schema response. */
export interface TableSchema {
  columns: ColumnInfo[];
  primaryKeys: string[];
}

/** @brief Table data response. */
export interface TableData {
  rows: Record<string, string | null>[];
  rowCount: number;
}

/** @brief Column definition for new table. */
export interface ColDef {
  name: string;
  type: string;
  primaryKey: boolean;
  notNull: boolean;
}
