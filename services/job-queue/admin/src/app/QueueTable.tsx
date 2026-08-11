'use client'

/**
 * Queue tab — pending, retrying, and
 * running rows with a one-click cancel
 * for rows that haven't started yet.
 */

import type { QueueRow } from
  '@/hooks/useJobs'

interface Props {
  rows: QueueRow[]
  onCancel: (id: number) => Promise<boolean>
}

export function QueueTable({
  rows, onCancel,
}: Props) {
  if (rows.length === 0) {
    return (
      <p data-testid="queue-empty">
        No queued jobs.
      </p>
    )
  }

  return (
    <table
      className="jobs-table"
      data-testid="queue-table"
    >
      <thead>
        <tr>
          <th>ID</th>
          <th>Name</th>
          <th>Handler</th>
          <th>Status</th>
          <th>Attempts</th>
          <th>Run at</th>
          <th>Worker</th>
          <th />
        </tr>
      </thead>
      <tbody>
        {rows.map(r => (
          <tr key={r.id}>
            <td>{r.id}</td>
            <td>{r.name}</td>
            <td>{r.handler}</td>
            <td data-status={r.status}>
              {r.status}
            </td>
            <td>
              {r.attempts}/{r.max_attempts}
            </td>
            <td>{r.run_at}</td>
            <td>
              {r.locked_by ?? '—'}
            </td>
            <td>
              {r.status !== 'running' && (
                <button
                  type="button"
                  aria-label={
                    `Cancel job ${r.id}`
                  }
                  onClick={() =>
                    onCancel(r.id)
                  }
                >
                  Cancel
                </button>
              )}
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
