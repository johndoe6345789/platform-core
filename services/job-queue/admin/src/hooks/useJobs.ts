'use client'

/**
 * Jobs operator hook — polls the
 * job-scheduler REST API (/api/jobs)
 * for queue, runs, and dead-letter
 * views.  Also exposes action helpers
 * for enqueue / retry / cancel.
 */

import {
  useState, useEffect, useCallback,
} from 'react'

const API = '/api/jobs'
const POLL_MS = 5_000

export interface QueueRow {
  id: number
  name: string
  handler: string
  priority: number
  status: string
  attempts: number
  max_attempts: number
  run_at: string
  locked_by: string | null
}

export interface RunRow {
  id: number
  job_queue_id: number | null
  name: string
  handler: string
  attempt: number
  status: string
  started_at: string
  finished_at: string | null
  duration_ms: number | null
  worker_id: string | null
}

export interface DeadRow {
  id: number
  original_id: number
  name: string
  handler: string
  attempts: number
  last_error: string
  failed_at: string
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

export function useJobs() {
  const [queue, setQueue] =
    useState<QueueRow[]>([])
  const [runs, setRuns] =
    useState<RunRow[]>([])
  const [dead, setDead] =
    useState<DeadRow[]>([])

  const refresh = useCallback(async () => {
    const q = await getJson<
      { items: QueueRow[] }
    >('/queue')
    const r = await getJson<
      { items: RunRow[] }
    >('/runs')
    const d = await getJson<
      { items: DeadRow[] }
    >('/dead_letter')
    if (q) setQueue(q.items)
    if (r) setRuns(r.items)
    if (d) setDead(d.items)
  }, [])

  useEffect(() => {
    refresh()
    const id = setInterval(refresh, POLL_MS)
    return () => clearInterval(id)
  }, [refresh])

  return { queue, runs, dead, refresh }
}
