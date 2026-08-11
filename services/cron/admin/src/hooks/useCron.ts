'use client'

/**
 * Cron schedule hook — polls /api/cron
 * for schedule rows and exposes CRUD +
 * preview helpers used by the page.
 */

import {
  useState, useEffect, useCallback,
} from 'react'

const API = '/api/cron'
const POLL_MS = 10_000

export interface ScheduleRow {
  id: number
  name: string
  cron: string
  handler: string
  enabled: boolean
  next_run_at: string
  last_run_at: string
  description: string
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

export function useCron() {
  const [items, setItems] =
    useState<ScheduleRow[]>([])

  const refresh = useCallback(async () => {
    const d = await getJson<
      { items: ScheduleRow[] }
    >('/schedules')
    if (d) setItems(d.items)
  }, [])

  useEffect(() => {
    refresh()
    const id = setInterval(refresh, POLL_MS)
    return () => clearInterval(id)
  }, [refresh])

  const toggle = useCallback(
    async (row: ScheduleRow) => {
      await fetch(
        `${API}/schedules/${row.id}`,
        {
          method: 'PUT',
          headers: {
            'Content-Type':
              'application/json',
          },
          body: JSON.stringify({
            enabled: !row.enabled,
          }),
        },
      )
      await refresh()
    },
    [refresh],
  )

  const fireNow = useCallback(
    async (id: number) => {
      await fetch(
        `${API}/schedules/${id}/fire`,
        { method: 'POST' },
      )
      await refresh()
    },
    [refresh],
  )

  return { items, refresh, toggle, fireNow }
}
