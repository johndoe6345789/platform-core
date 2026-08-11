'use client'

/**
 * Feature flags operator page.
 */

import { useState } from 'react'
import {
  Typography, Alert, Box,
} from '@shared/m3'
import { useFlags } from '@/hooks/useFlags'
import {
  useFlagMutations,
} from '@/hooks/useFlagMutations'
import type { FlagRow } from '@/hooks/useFlags'
import { FlagTable } from './FlagTable'
import { FlagEditor } from './FlagEditor'
import { AuditDrawer } from './AuditDrawer'

export default function FlagsPage() {
  const { flags, loading, error, refresh } =
    useFlags()
  const { patchFlag, pending } = useFlagMutations()
  const [editing, setEditing] =
    useState<FlagRow | null>(null)
  const [auditKey, setAuditKey] =
    useState<string | null>(null)

  const onSave = async (p: {
    description: string
    enabled: boolean
    rollout_pct: number
  }) => {
    if (!editing) return
    await patchFlag(editing.key, p)
    setEditing(null)
    refresh()
  }

  const onToggle = async (f: FlagRow) => {
    await patchFlag(f.key, { enabled: !f.enabled })
    refresh()
  }

  return (
    <Box className="flags-shell">
      <Box className="flags-header">
        <Box className="flags-title">
          <Typography variant="h4">
            Feature flags
          </Typography>
        </Box>
      </Box>
      {error && (
        <Alert severity="error">{error}</Alert>
      )}
      {loading ? (
        <Typography>Loading…</Typography>
      ) : (
        <FlagTable
          flags={flags}
          onToggle={onToggle}
          onEdit={setEditing}
          onAudit={f => setAuditKey(f.key)}
        />
      )}
      <FlagEditor
        flag={editing}
        open={!!editing}
        pending={pending}
        onClose={() => setEditing(null)}
        onSave={onSave}
      />
      <AuditDrawer
        flagKey={auditKey}
        open={!!auditKey}
        onClose={() => setAuditKey(null)}
      />
    </Box>
  )
}
