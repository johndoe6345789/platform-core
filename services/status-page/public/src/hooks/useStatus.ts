'use client'

/**
 * useStatus — polls /api/healthz every 15s and
 * exposes a normalised list of service entries
 * for the StatusGrid component.
 */

import { useEffect, useState } from 'react'

/** Single service row from /api/healthz. */
export interface ServiceStatus {
  name: string
  ok: boolean
  latencyMs?: number
  message?: string
}

interface StatusState {
  services: ServiceStatus[]
  loading: boolean
  error: string | null
}

const HEALTHZ = '/api/healthz'
const POLL_MS = 15000

export function useStatus(): StatusState {
  const [state, setState] = useState<StatusState>({
    services: [],
    loading: true,
    error: null,
  })

  useEffect(() => {
    let cancelled = false
    const tick = async () => {
      try {
        const r = await fetch(HEALTHZ, {
          cache: 'no-store',
        })
        const j = await r.json()
        const rows: ServiceStatus[] =
          Array.isArray(j.services)
            ? j.services
            : []
        if (!cancelled) {
          setState({
            services: rows,
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
    tick()
    const id = setInterval(tick, POLL_MS)
    return () => {
      cancelled = true
      clearInterval(id)
    }
  }, [])

  return state
}
