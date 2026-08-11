'use client'

/**
 * @file useIntegrity.ts
 * @brief Walks the audit hash chain via
 *        the backend verifier endpoint.
 */

import {
  useState, useCallback,
} from 'react'

/** Result shape returned by /api/audit/verify. */
export interface IntegrityReport {
  ok: boolean
  rows_checked: number
  first_bad_id?: number
  message: string
}

const API_BASE = '/audit/api/audit'

/**
 * @brief React hook that on-demand walks
 *        the hash chain for a tenant.
 * @param tenant UUID string.
 */
export function useIntegrity(tenant: string) {
  const [report, setReport] =
    useState<IntegrityReport | null>(null)
  const [loading, setLoading] =
    useState(false)
  const [error, setError] =
    useState<string | null>(null)

  const verify = useCallback(async () => {
    if (!tenant) return
    setLoading(true)
    setError(null)
    try {
      const r = await fetch(
        `${API_BASE}/verify` +
        `?tenant=${encodeURIComponent(tenant)}`,
      )
      if (!r.ok) {
        throw new Error(`HTTP ${r.status}`)
      }
      const data =
        (await r.json()) as IntegrityReport
      setReport(data)
    } catch (ex) {
      setError(
        ex instanceof Error
          ? ex.message
          : 'unknown error',
      )
      setReport(null)
    } finally {
      setLoading(false)
    }
  }, [tenant])

  return { report, loading, error, verify }
}
