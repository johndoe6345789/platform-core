'use client'

/**
 * Fetches audit history for a single flag.
 */

import { useCallback, useEffect, useState } from 'react'

/** Single audit row shape. */
export interface AuditEntry {
  actor: string
  prev: Record<string, unknown>
  next: Record<string, unknown>
  at: string
}

const API = '/flags/api/flags'

/** Hook returning audit rows for the given key. */
export function useFlagAudit(key: string | null) {
  const [audit, setAudit] = useState<AuditEntry[]>([])
  const [loading, setLoading] = useState(false)

  const refresh = useCallback(async () => {
    if (!key) {
      setAudit([])
      return
    }
    setLoading(true)
    try {
      const r = await fetch(
        `${API}/${encodeURIComponent(key)}/audit`,
        { credentials: 'include' },
      )
      if (!r.ok) {
        setAudit([])
        return
      }
      const data = await r.json()
      setAudit(data.audit ?? [])
    } finally {
      setLoading(false)
    }
  }, [key])

  useEffect(() => {
    refresh()
  }, [refresh])

  return { audit, loading, refresh }
}
