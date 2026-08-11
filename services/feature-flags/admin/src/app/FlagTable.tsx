'use client'

/**
 * Table of all feature flags with inline
 * enable/disable toggles.
 */

import {
  Table, TableBody, TableCell,
  TableContainer, TableHead, TableRow,
  Switch, Chip, Typography, Button,
} from '@shared/m3'
import type { FlagRow } from '@/hooks/useFlags'

interface Props {
  flags: FlagRow[]
  onToggle: (f: FlagRow) => void
  onEdit: (f: FlagRow) => void
  onAudit: (f: FlagRow) => void
}

export function FlagTable({
  flags, onToggle, onEdit, onAudit,
}: Props) {
  if (flags.length === 0) {
    return (
      <div className="flags-empty">
        <Typography variant="body1">
          No feature flags configured
        </Typography>
      </div>
    )
  }

  return (
    <TableContainer data-testid="flags-table">
      <Table aria-label="Feature flags">
        <TableHead>
          <TableRow>
            <TableCell>Key</TableCell>
            <TableCell>Description</TableCell>
            <TableCell>Enabled</TableCell>
            <TableCell>Rollout</TableCell>
            <TableCell>Actions</TableCell>
          </TableRow>
        </TableHead>
        <TableBody>
          {flags.map(f => (
            <TableRow
              key={f.key}
              data-testid={`flag-row-${f.key}`}
            >
              <TableCell>
                <code>{f.key}</code>
              </TableCell>
              <TableCell>{f.description}</TableCell>
              <TableCell>
                <Switch
                  checked={f.enabled}
                  onChange={() => onToggle(f)}
                  aria-label={
                    `Toggle ${f.key}`
                  }
                />
              </TableCell>
              <TableCell>
                <Chip
                  label={`${f.rollout_pct}%`}
                  size="small"
                />
              </TableCell>
              <TableCell>
                <Button
                  size="small"
                  onClick={() => onEdit(f)}
                >
                  Edit
                </Button>
                <Button
                  size="small"
                  onClick={() => onAudit(f)}
                >
                  Audit
                </Button>
              </TableCell>
            </TableRow>
          ))}
        </TableBody>
      </Table>
    </TableContainer>
  )
}
