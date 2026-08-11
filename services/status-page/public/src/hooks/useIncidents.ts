'use client'

/**
 * useIncidents — pulls /api/status/incidents and
 * /api/status/history from the backend. Both are
 * public (unauthenticated) endpoints served by
 * StatusController on the main backend daemon.
 */

import { useEffect, useState } from 'react'

/** Incident record returned by StatusController. */
export interface Incident {
  id: number
  title: string
  body: string
  severity: 'minor' | 'major' | 'critical'
  status:
    | 'investigating'
    | 'identified'
    | 'monitoring'
    | 'resolved'
  started_at: string
  resolved_at?: string
}

interface IncidentsState {
  active: Incident[]
  history: Incident[]
  loading: boolean
  error: string | null
}

export function useIncidents(): IncidentsState {
  const [state, setState] =
    useState<IncidentsState>({
      active: [],
      history: [],
      loading: true,
      error: null,
    })

  useEffect(() => {
    let cancelled = false
    const load = async () => {
      try {
        const [a, h] = await Promise.all([
          fetch('/api/status/incidents'),
          fetch('/api/status/history'),
        ])
        const aj = await a.json()
        const hj = await h.json()
        if (!cancelled) {
          setState({
            active: aj.incidents ?? [],
            history: hj.history ?? [],
            loading: false,
            error: null,
          })
        }
      } catch (e) {
        if (!cancelled) {
          setState((prev) => ({
            ...prev,
            loading: false,
            error:
              e instanceof Error
                ? e.message
                : 'fetch failed',
          }))
        }
      }
    }
    load()
  }, [])

  return state
}
