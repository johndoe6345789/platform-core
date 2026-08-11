/** @brief ORDER BY + LIMIT query controls. */

/** @brief Props for QueryOptions molecule. */
interface QueryOptionsProps {
  columns: string[];
  orderCol: string;
  setOrderCol: (v: string) => void;
  orderDir: 'ASC' | 'DESC';
  setOrderDir: (v: 'ASC' | 'DESC') => void;
  limit: number;
  setLimit: (v: number) => void;
  onExecute: () => void;
  loading: boolean;
  table: string;
}

/**
 * @brief Controls for ORDER BY column/direction,
 * LIMIT, and query execution.
 */
export default function QueryOptions({
  columns, orderCol, setOrderCol,
  orderDir, setOrderDir,
  limit, setLimit,
  onExecute, loading, table,
}: QueryOptionsProps) {
  return (
    <>
      <div>
        <label htmlFor="order-col">
          Order By
        </label>
        <select
          id="order-col"
          value={orderCol}
          onChange={
            (e) => setOrderCol(e.target.value)
          }
          aria-label="Order by column"
          data-testid="order-col-select"
        >
          <option value="">None</option>
          {columns.map((c) => (
            <option key={c} value={c}>
              {c}
            </option>
          ))}
        </select>
        <select
          value={orderDir}
          onChange={
            (e) => setOrderDir(
              e.target.value as 'ASC' | 'DESC',
            )
          }
          aria-label="Sort direction"
          data-testid="order-dir-select"
        >
          <option value="ASC">ASC</option>
          <option value="DESC">DESC</option>
        </select>
      </div>
      <div>
        <label htmlFor="limit">Limit</label>
        <input
          id="limit"
          type="number"
          value={limit}
          onChange={
            (e) => setLimit(
              Number(e.target.value),
            )
          }
          aria-label="Row limit"
          data-testid="limit-input"
        />
      </div>
      <button
        onClick={onExecute}
        disabled={loading || !table}
        data-testid="qb-execute"
        aria-label="Execute query"
      >
        {loading ? 'Running...' : 'Execute'}
      </button>
    </>
  );
}
