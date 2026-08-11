'use client'

/**
 * Drawer showing audit history for a flag.
 */

import {
  Drawer, Typography, Divider, Box,
} from '@shared/m3'
import { useFlagAudit } from '@/hooks/useFlagAudit'

interface Props {
  flagKey: string | null
  open: boolean
  onClose: () => void
}

export function AuditDrawer({
  flagKey, open, onClose,
}: Props) {
  const { audit, loading } =
    useFlagAudit(open ? flagKey : null)

  return (
    <Drawer
      anchor="right"
      open={open}
      onClose={onClose}
    >
      <Box className="flags-audit">
        <Typography variant="h6">
          Audit — {flagKey ?? ''}
        </Typography>
        <Divider />
        {loading && (
          <Typography variant="body2">
            Loading…
          </Typography>
        )}
        {!loading && audit.length === 0 && (
          <Typography variant="body2">
            No audit entries yet.
          </Typography>
        )}
        {audit.map((a, i) => (
          <Box
            key={i}
            className="flags-audit-row"
            data-testid={`audit-${i}`}
          >
            <Typography variant="caption">
              {a.at} — {a.actor || 'system'}
            </Typography>
            <pre style={{
              fontSize: '0.75rem',
              margin: 0,
              whiteSpace: 'pre-wrap',
            }}>
              {JSON.stringify(a.next, null, 2)}
            </pre>
          </Box>
        ))}
      </Box>
    </Drawer>
  )
}
