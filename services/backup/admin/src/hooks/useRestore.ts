'use client'

/**
 * Restore + trigger helpers for the backups
 * operator tool.  Wraps POST /api/backups/trigger
 * and POST /api/backups/restore into two async
 * functions the UI can call from button handlers.
 */

import { useCallback } from 'react'

const API = '/api/backups'

export interface TriggerResult {
  id: number
  status: string
  bytes: number
  s3_key: string
  error: string
  kind: string
}

async function postJson<T>(
  path: string,
  body: unknown,
): Promise<T | null> {
  try {
    const r = await fetch(`${API}${path}`, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json',
      },
      body: JSON.stringify(body),
    })
    if (!r.ok) return null
    return (await r.json()) as T
  } catch {
    return null
  }
}

export function useRestore(
  onDone: () => void,
) {
  const trigger = useCallback(
    async (kind: string) => {
      const r = await postJson<TriggerResult>(
        '/trigger',
        { kind },
      )
      onDone()
      return r
    },
    [onDone],
  )

  const restore = useCallback(
    async (s3Key: string) => {
      const r = await postJson<{ id: number }>(
        '/restore',
        { s3_key: s3Key },
      )
      onDone()
      return r
    },
    [onDone],
  )

  return { trigger, restore }
}
