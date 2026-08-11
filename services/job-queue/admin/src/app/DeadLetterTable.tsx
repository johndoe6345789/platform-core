'use client'

/**
 * Dead-letter tab — jobs that exhausted
 * retries, with a one-click requeue that
 * copies the payload into a fresh
 * job_queue row at priority 50.
 */

import type { DeadRow } from '@/hooks/useJobs'

interface Props {
  rows: DeadRow[]
  onRetry: (id: number) => Promise<boolean>
}

export function DeadLetterTable({
  rows, onRetry,
}: Props) {
  if (rows.length === 0) {
    return (
      <p data-testid="dead-empty">
        Dead letter queue is empty.
      </p>
    )
  }

  return (
    <table
      className="jobs-table"
      data-testid="dead-table"
    >
      <thead>
        <tr>
          <th>ID</th>
          <th>Name</th>
          <th>Handler</th>
          <th>Attempts</th>
          <th>Failed at</th>
          <th>Error</th>
          <th />
        </tr>
      </thead>
      <tbody>
        {rows.map(r => (
          <tr key={r.id}>
            <td>{r.id}</td>
            <td>{r.name}</td>
            <td>{r.handler}</td>
            <td>{r.attempts}</td>
            <td>{r.failed_at}</td>
            <td data-status="dead">
              {r.last_error}
            </td>
            <td>
              <button
                type="button"
                aria-label={
                  `Requeue job ${r.id}`
                }
                onClick={() =>
                  onRetry(r.id)
                }
              >
                Requeue
              </button>
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
