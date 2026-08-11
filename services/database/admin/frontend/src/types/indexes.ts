/** @brief Index types. */

/** @brief Index entry from pg_indexes. */
export interface IndexEntry {
  indexname: string;
  indexdef: string;
}

/** @brief Request to create an index. */
export interface CreateIndexRequest {
  tableName: string;
  indexName: string;
  indexType: string;
  columns: string[];
  unique: boolean;
}
