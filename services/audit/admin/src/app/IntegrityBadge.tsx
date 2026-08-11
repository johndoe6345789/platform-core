'use client'

/**
 * @file IntegrityBadge.tsx
 * @brief Visual pill showing hash-chain
 *        verification status.
 */

import type {
  IntegrityReport,
} from '@/hooks/useIntegrity'

/** Props for IntegrityBadge. */
export interface IntegrityBadgeProps {
  report: IntegrityReport | null
  loading: boolean
}

/**
 * @brief Status pill for chain integrity.
 * @param props Component props.
 */
export default function IntegrityBadge(
  { report, loading }: IntegrityBadgeProps,
) {
  if (loading) {
    return (
      <span
        className="integrity-badge"
        data-testid="integrity-loading"
        aria-label="verifying integrity"
      >
        Verifying…
      </span>
    )
  }
  if (!report) {
    return (
      <span
        className="integrity-badge"
        data-testid="integrity-idle"
        aria-label="integrity not checked"
      >
        Not verified
      </span>
    )
  }
  const cls = report.ok
    ? 'integrity-badge integrity-ok'
    : 'integrity-badge integrity-bad'
  const label = report.ok
    ? `Chain OK (${report.rows_checked} rows)`
    : `Chain broken @ ${report.first_bad_id}`
  return (
    <span
      className={cls}
      data-testid="integrity-badge"
      aria-label={label}
    >
      {label}
    </span>
  )
}
