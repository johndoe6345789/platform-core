'use client'

/**
 * Fetches the full feature-flag list from the
 * backend admin API with polling refresh.
 */

import {
  useCallback, useEffect, useState,
} from 'react'

/** Shape of a feature flag row. */
export interface FlagRow {
  id: number
  key: string
  description: string
  enabled: boolean
  rollout_pct: number
  targeting: Record<string, unknown>
  updated_at: string
}

const API = '/flags/api/flags'

/** Hook returning the flag list + refresh fn. */
export function useFlags() {
  const [flags, setFlags] = useState<FlagRow[]>([])
  const [loading, setLoading] = useState(true)
  const [error, setError] =
    useState<string | null>(null)

  const refresh = useCallback(async () => {
    try {
      const r = await fetch(API, {
        credentials: 'include',
      })
      if (!r.ok) {
        throw new Error(`HTTP ${r.status}`)
      }
      const data = await r.json()
      setFlags(data.flags ?? [])
      setError(null)
    } catch (e) {
      setError(
        e instanceof Error ? e.message : 'error',
      )
    } finally {
      setLoading(false)
    }
  }, [])

  useEffect(() => {
    refresh()
  }, [refresh])

  return { flags, loading, error, refresh }
}
