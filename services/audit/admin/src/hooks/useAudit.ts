'use client'

/**
 * @file useAudit.ts
 * @brief Fetch audit events for a tenant
 *        within a time window.
 */

import {
  useState, useEffect, useCallback,
} from 'react'

/** Single audit event row. */
export interface AuditEvent {
  id: number
  tenant_id: string
  actor_id?: string
  action: string
  target_type?: string
  target_id?: string
  payload: Record<string, unknown>
  prev_hash: string
  hash: string
  created_at: string
}

/** Query parameters accepted by useAudit. */
export interface AuditQuery {
  tenant: string
  since?: string
  until?: string
  limit?: number
}

const API_BASE = '/audit/api/audit'

/**
 * @brief React hook that keeps an audit
 *        event list in sync with the API.
 * @param query Tenant + time window.
 */
export function useAudit(query: AuditQuery) {
  const [events, setEvents] =
    useState<AuditEvent[]>([])
  const [loading, setLoading] =
    useState(true)
  const [error, setError] =
    useState<string | null>(null)

  const refresh = useCallback(async () => {
    setLoading(true)
    setError(null)
    try {
      const params = new URLSearchParams()
      params.set('tenant', query.tenant)
      if (query.since)
        params.set('since', query.since)
      if (query.until)
        params.set('until', query.until)
      params.set(
        'limit',
        String(query.limit ?? 100),
      )
      const r = await fetch(
        `${API_BASE}/events?${params}`,
      )
      if (!r.ok) {
        throw new Error(
          `HTTP ${r.status}`,
        )
      }
      const data = await r.json()
      setEvents(data as AuditEvent[])
    } catch (ex) {
      setError(
        ex instanceof Error
          ? ex.message
          : 'unknown error',
      )
      setEvents([])
    } finally {
      setLoading(false)
    }
  }, [
    query.tenant,
    query.since,
    query.until,
    query.limit,
  ])

  useEffect(() => {
    if (query.tenant) refresh()
  }, [query.tenant, refresh])

  return { events, loading, error, refresh }
}
