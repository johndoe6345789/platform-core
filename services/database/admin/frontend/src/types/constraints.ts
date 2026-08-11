/** @brief Constraint types. */

/** @brief Constraint entry from the DB. */
export interface ConstraintEntry {
  constraint_name: string;
  constraint_type: string;
  column_name: string;
}

/** @brief Request to add a constraint. */
export interface AddConstraintRequest {
  tableName: string;
  type: 'UNIQUE' | 'NOT NULL' | 'CHECK';
  columnName: string;
  expression?: string;
}
