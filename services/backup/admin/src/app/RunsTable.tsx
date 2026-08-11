'use client'

/**
 * Runs tab — recent backup_runs rows with
 * status, size, kind, and a "restore"
 * button when the row has an s3_key.
 */

import type { RunRow } from '@/hooks/useBackups'

interface Props {
  rows: RunRow[]
  onRestore: (key: string) => void
}

export function RunsTable(
  { rows, onRestore }: Props,
) {
  if (rows.length === 0) {
    return (
      <p data-testid="runs-empty">
        No backup runs yet.
      </p>
    )
  }

  return (
    <table
      className="backups-table"
      data-testid="runs-table"
      aria-label="Backup run history"
    >
      <thead>
        <tr>
          <th>ID</th>
          <th>Started</th>
          <th>Finished</th>
          <th>Kind</th>
          <th>Status</th>
          <th>Bytes</th>
          <th>Key</th>
          <th>Action</th>
        </tr>
      </thead>
      <tbody>
        {rows.map(r => (
          <tr key={r.id}>
            <td>{r.id}</td>
            <td>{r.started_at}</td>
            <td>{r.finished_at ?? '—'}</td>
            <td>{r.kind}</td>
            <td data-status={r.status}>
              {r.status}
            </td>
            <td>{r.bytes}</td>
            <td>{r.s3_key ?? '—'}</td>
            <td>
              {r.s3_key && (
                <button
                  type="button"
                  data-testid={
                    `restore-${r.id}`
                  }
                  aria-label={
                    'Restore backup ' + r.id
                  }
                  onClick={() =>
                    onRestore(r.s3_key!)
                  }
                >
                  Restore
                </button>
              )}
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
