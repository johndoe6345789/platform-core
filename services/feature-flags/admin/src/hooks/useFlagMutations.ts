'use client'

/**
 * Mutation hook for patching feature flags.
 */

import { useCallback, useState } from 'react'

/** Partial patch for a feature flag. */
export interface FlagPatch {
  description?: string
  enabled?: boolean
  rollout_pct?: number
  targeting?: Record<string, unknown>
}

const API = '/flags/api/flags'

/** Hook returning a patchFlag fn + pending flag. */
export function useFlagMutations() {
  const [pending, setPending] = useState(false)
  const [error, setError] =
    useState<string | null>(null)

  const patchFlag = useCallback(
    async (key: string, patch: FlagPatch) => {
      setPending(true)
      setError(null)
      try {
        const r = await fetch(
          `${API}/${encodeURIComponent(key)}`,
          {
            method: 'PATCH',
            credentials: 'include',
            headers: {
              'Content-Type': 'application/json',
            },
            body: JSON.stringify(patch),
          },
        )
        if (!r.ok) {
          throw new Error(`HTTP ${r.status}`)
        }
        return await r.json()
      } catch (e) {
        setError(
          e instanceof Error
            ? e.message : 'error',
        )
        throw e
      } finally {
        setPending(false)
      }
    }, [],
  )

  return { patchFlag, pending, error }
}
