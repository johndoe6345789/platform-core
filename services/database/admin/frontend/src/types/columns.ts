/** @brief Column management types. */

/** @brief Request to add a column. */
export interface AddColumnRequest {
  tableName: string;
  columnName: string;
  dataType: string;
  notNull?: boolean;
  default?: string;
}

/** @brief Request to modify a column type. */
export interface ModifyColumnRequest {
  tableName: string;
  columnName: string;
  newType: string;
}

/** @brief Request to drop a column. */
export interface DropColumnRequest {
  tableName: string;
  columnName: string;
}
