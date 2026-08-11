'use client'

/**
 * Policy tab — read-only view of the
 * backup_policy table.  Future work will
 * wire PATCH /api/backups/policy/:id to
 * make this an inline editor.
 */

import type {
  PolicyRow,
} from '@/hooks/useBackups'

interface Props {
  rows: PolicyRow[]
}

export function PolicyEditor({ rows }: Props) {
  if (rows.length === 0) {
    return (
      <p data-testid="policy-empty">
        No policies configured.
      </p>
    )
  }

  return (
    <table
      className="backups-table"
      data-testid="policy-table"
      aria-label="Backup policies"
    >
      <thead>
        <tr>
          <th>Name</th>
          <th>Kind</th>
          <th>Retain (days)</th>
          <th>Schedule</th>
        </tr>
      </thead>
      <tbody>
        {rows.map(p => (
          <tr key={p.id}>
            <td>{p.name}</td>
            <td>{p.kind}</td>
            <td>{p.retain_days}</td>
            <td>
              <code>{p.schedule_cron}</code>
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
