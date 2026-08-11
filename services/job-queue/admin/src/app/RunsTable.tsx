'use client'

/**
 * Runs tab — recent job_runs rows with
 * attempt, status, duration, and the
 * worker id that processed each row.
 */

import type { RunRow } from '@/hooks/useJobs'

interface Props {
  rows: RunRow[]
}

export function RunsTable({ rows }: Props) {
  if (rows.length === 0) {
    return (
      <p data-testid="runs-empty">
        No run history.
      </p>
    )
  }

  return (
    <table
      className="jobs-table"
      data-testid="runs-table"
    >
      <thead>
        <tr>
          <th>ID</th>
          <th>Name</th>
          <th>Handler</th>
          <th>Attempt</th>
          <th>Status</th>
          <th>Started</th>
          <th>Duration</th>
          <th>Worker</th>
        </tr>
      </thead>
      <tbody>
        {rows.map(r => (
          <tr key={r.id}>
            <td>{r.id}</td>
            <td>{r.name}</td>
            <td>{r.handler}</td>
            <td>{r.attempt}</td>
            <td data-status={r.status}>
              {r.status}
            </td>
            <td>{r.started_at}</td>
            <td>
              {r.duration_ms != null
                ? `${r.duration_ms} ms`
                : '—'}
            </td>
            <td>
              {r.worker_id ?? '—'}
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
