'use client'

/**
 * @file EventTable.tsx
 * @brief Renders a paginated list of audit
 *        events with shortened hash cells.
 */

import type { AuditEvent } from '@/hooks/useAudit'

/** Props for EventTable. */
export interface EventTableProps {
  events: AuditEvent[]
}

const short = (h: string): string =>
  h ? `${h.slice(0, 10)}…` : '—'

/**
 * @brief Audit event table.
 * @param props Component props.
 */
export default function EventTable(
  { events }: EventTableProps,
) {
  if (events.length === 0) {
    return (
      <div
        className="audit-empty"
        data-testid="audit-empty"
        aria-label="no audit events"
      >
        No audit events in this window.
      </div>
    )
  }

  return (
    <table
      className="audit-table"
      data-testid="audit-event-table"
      aria-label="audit event table"
    >
      <thead>
        <tr>
          <th>ID</th>
          <th>Time</th>
          <th>Action</th>
          <th>Actor</th>
          <th>Target</th>
          <th>Hash</th>
        </tr>
      </thead>
      <tbody>
        {events.map((e) => (
          <tr key={e.id}>
            <td>{e.id}</td>
            <td>{e.created_at}</td>
            <td>{e.action}</td>
            <td>{e.actor_id ?? '—'}</td>
            <td>
              {e.target_type ?? '—'}
              {e.target_id
                ? `:${e.target_id}`
                : ''}
            </td>
            <td className="audit-hash">
              {short(e.hash)}
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  )
}
