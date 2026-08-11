'use client'

/**
 * Job write-action hook — kept separate
 * from useJobs so read-only tab views
 * don't pull the mutation helpers into
 * their bundle.
 */

import { useCallback } from 'react'

const API = '/api/jobs'

export interface EnqueueBody {
  name: string
  handler: string
  payload?: unknown
  priority?: number
  max_attempts?: number
  backoff?: 'exponential' | 'linear' | 'fixed'
}

async function send(
  path: string,
  method: string,
  body?: unknown,
): Promise<boolean> {
  try {
    const r = await fetch(
      `${API}${path}`,
      {
        method,
        headers: body
          ? {
              'Content-Type':
                'application/json',
            }
          : undefined,
        body: body
          ? JSON.stringify(body)
          : undefined,
      },
    )
    return r.ok
  } catch {
    return false
  }
}

export function useJobActions(
  onChange?: () => void,
) {
  const enqueue = useCallback(
    async (body: EnqueueBody) => {
      const ok = await send(
        '/enqueue', 'POST', body,
      )
      if (ok) onChange?.()
      return ok
    },
    [onChange],
  )

  const retry = useCallback(
    async (id: number) => {
      const ok = await send(
        `/retry_dead/${id}`, 'POST',
      )
      if (ok) onChange?.()
      return ok
    },
    [onChange],
  )

  const cancel = useCallback(
    async (id: number) => {
      const ok = await send(
        `/queue/${id}`, 'DELETE',
      )
      if (ok) onChange?.()
      return ok
    },
    [onChange],
  )

  return { enqueue, retry, cancel }
}
