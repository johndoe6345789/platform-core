'use client'

/**
 * UptimeBar — a 90-cell horizontal bar used as a
 * visual summary of daily uptime. Since the
 * template does not yet record per-day history,
 * the component defaults to all-up and accepts an
 * optional array of statuses for future wiring.
 */

interface Props {
  /** Array of daily states, oldest → newest. */
  days?: Array<'up' | 'degraded' | 'down'>
}

const DEFAULT_DAYS = 90

export function UptimeBar({ days }: Props) {
  const cells: Array<'up' | 'degraded' | 'down'> =
    days && days.length > 0
      ? days
      : Array.from(
          { length: DEFAULT_DAYS },
          () => 'up' as const,
        )
  return (
    <div
      className="status-uptime"
      role="img"
      data-testid="uptime-bar"
      aria-label={
        `Uptime for last ${cells.length} days`
      }
    >
      {cells.map((c, i) => (
        <span
          key={i}
          className={
            c === 'up'
              ? 'status-uptime-cell'
              : `status-uptime-cell ${c}`
          }
        />
      ))}
    </div>
  )
}
