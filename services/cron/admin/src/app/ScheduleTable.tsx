'use client'

/**
 * Schedule table — one row per
 * scheduled_jobs entry with inline
 * toggle + fire-now controls.
 */

import type { ScheduleRow } from
  '@/hooks/useCron'

interface Props {
  rows: ScheduleRow[]
  onToggle:
    (row: ScheduleRow) => Promise<void>
  onFireNow:
    (id: number) => Promise<void>
}

export function ScheduleTable({
  rows, onToggle, onFireNow,
}: Props) {
  if (rows.length === 0) {
    return (
      <p data-testid="cron-empty">
        No schedules yet.
      </p>
    )
  }

  return (
    <table
      className="cron-table"
      data-testid="cron-table"
    >
      <thead>
        <tr>
          <th>Name</th>
          <th>Cron</th>
          <th>Handler</th>
          <th>Next run</th>
          <th>Last run</th>
          <th>Enabled</th>
          <th />
        </tr>
      </thead>
      <tbody>
        {rows.map(r => (
          <tr
            key={r.id}
            data-disabled={!r.enabled}
          >
            <td>{r.name}</td>
            <td>
              <code>{r.cron}</code>
            </td>
            <td>{r.handler}</td>
            <td>
              {r.next_run_at || '—'}
            </td>
            <td>
              {r.last_run_at || '—'}
            </td>
            <td>
              <input
                type="checkbox"
                checked={r.enabled}
                aria-label={
                  `Enable ${r.name}`
                }
                onChange={() =>
                  onToggle(r)
                }
              />
            </td>
            <td>
              <button
                type="button"
                aria-label={
                  `Fire ${r.name} now`
                }
                onClick={() =>
                  onFireNow(r.id)
                }
              >
                Run now
              </button>
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
