/** @brief Query-related types. */

/** @brief Result from a SQL query execution. */
export interface QueryResult {
  rows: Record<string, string | null>[];
  rowCount: number;
  query?: string;
}

/** @brief WHERE condition for query builder. */
export interface WhereCondition {
  column: string;
  operator: string;
  value: string;
}

/** @brief Query builder parameters. */
export interface QueryBuilderParams {
  table: string;
  columns?: string[];
  where?: WhereCondition[];
  orderBy?: {
    column: string;
    direction: 'ASC' | 'DESC';
  };
  limit?: number;
  offset?: number;
}
