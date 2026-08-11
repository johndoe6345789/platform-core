'use client'

/**
 * Backups operator hook — polls /api/backups
 * for the runs and policy tables and exposes
 * a refresh helper for manual refetches.
 */

import {
  useState, useEffect, useCallback,
} from 'react'

const API = '/api/backups'
const POLL_MS = 10_000

export interface RunRow {
  id: string
  started_at: string
  finished_at: string | null
  status: string
  bytes: string
  s3_key: string | null
  error: string | null
  kind: string
}

export interface PolicyRow {
  id: string
  name: string
  kind: string
  retain_days: string
  schedule_cron: string
}

async function getJson<T>(
  path: string,
): Promise<T | null> {
  try {
    const r = await fetch(`${API}${path}`)
    if (!r.ok) return null
    return (await r.json()) as T
  } catch {
    return null
  }
}

export function useBackups() {
  const [runs, setRuns] =
    useState<RunRow[]>([])
  const [policies, setPolicies] =
    useState<PolicyRow[]>([])

  const refresh = useCallback(async () => {
    const r = await getJson<
      { items: RunRow[] }
    >('/runs')
    const p = await getJson<
      { items: PolicyRow[] }
    >('/policy')
    if (r) setRuns(r.items)
    if (p) setPolicies(p.items)
  }, [])

  useEffect(() => {
    refresh()
    const id = setInterval(refresh, POLL_MS)
    return () => clearInterval(id)
  }, [refresh])

  return { runs, policies, refresh }
}
