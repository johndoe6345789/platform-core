'use client';

import { useState } from 'react';
import { useQueryExec } from '@/hooks/useQueryExec';
import { DataGrid } from '@/components/molecules';
import { Spinner } from '@/components/atoms';

/** @brief Panel for executing raw SQL queries. */
export default function SQLQueryPanel() {
  const [sql, setSql] = useState(
    'SELECT table_name FROM information_schema.tables '
    + "WHERE table_schema = 'public' "
    + 'ORDER BY table_name',
  );
  const { result, loading, error, execute } =
    useQueryExec();

  const handleRun = () => {
    if (sql.trim()) execute(sql);
  };

  return (
    <div data-testid="sql-query-panel">
      <h2>SQL Query</h2>
      <textarea
        value={sql}
        onChange={(e) => setSql(e.target.value)}
        rows={6}
        aria-label="SQL query input"
        data-testid="sql-input"
        style={{ width: '100%', fontFamily: 'monospace' }}
      />
      <button
        onClick={handleRun}
        disabled={loading}
        data-testid="sql-run"
        aria-label="Run query"
      >
        {loading ? 'Running...' : 'Run Query'}
      </button>
      {error && (
        <p className="error" role="alert"
          aria-live="assertive">{error}</p>
      )}
      {loading && <Spinner />}
      {result && (
        <div>
          <p>{result.rowCount} rows returned</p>
          <DataGrid
            columns={
              result.rows[0]
                ? Object.keys(result.rows[0])
                : []
            }
            rows={result.rows}
          />
        </div>
      )}
    </div>
  );
}
