'use client'

/**
 * Cron expression preview hook — calls
 * POST /api/cron/preview and returns the
 * next N fire times (or an error string
 * if the expression is invalid).  Debounces
 * the request by 300 ms so typing in the
 * editor doesn't spam the backend.
 */

import {
  useState, useEffect,
} from 'react'

const API = '/api/cron/preview'
const DEBOUNCE_MS = 300

interface Preview {
  fires: number[]
  error: string | null
}

export function useCronPreview(
  expression: string,
  count = 5,
): Preview {
  const [state, setState] =
    useState<Preview>({
      fires: [], error: null,
    })

  useEffect(() => {
    if (!expression) {
      setState({ fires: [], error: null })
      return
    }
    const handle = setTimeout(async () => {
      try {
        const r = await fetch(API, {
          method: 'POST',
          headers: {
            'Content-Type':
              'application/json',
          },
          body: JSON.stringify({
            cron: expression,
            count,
          }),
        })
        if (!r.ok) {
          const e = await r.json()
          setState({
            fires: [],
            error: e?.error ?? 'invalid',
          })
          return
        }
        const data = await r.json()
        setState({
          fires: data.next ?? [],
          error: null,
        })
      } catch {
        setState({
          fires: [],
          error: 'network error',
        })
      }
    }, DEBOUNCE_MS)

    return () => clearTimeout(handle)
  }, [expression, count])

  return state
}
