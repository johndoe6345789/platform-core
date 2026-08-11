'use client'

/**
 * Dialog for editing a single feature flag.
 */

import { useEffect, useState } from 'react'
import {
  Dialog, DialogTitle, DialogContent,
  DialogActions, Button, TextField, Switch,
  FormControlLabel,
} from '@shared/m3'
import { RolloutSlider } from './RolloutSlider'
import type { FlagRow } from '@/hooks/useFlags'

interface Props {
  flag: FlagRow | null
  open: boolean
  onClose: () => void
  onSave: (patch: {
    description: string
    enabled: boolean
    rollout_pct: number
  }) => void
  pending?: boolean
}

export function FlagEditor({
  flag, open, onClose, onSave, pending,
}: Props) {
  const [desc, setDesc] = useState('')
  const [enabled, setEnabled] = useState(false)
  const [pct, setPct] = useState(0)

  useEffect(() => {
    if (!flag) return
    setDesc(flag.description)
    setEnabled(flag.enabled)
    setPct(flag.rollout_pct)
  }, [flag])

  if (!flag) return null

  return (
    <Dialog open={open} onClose={onClose}>
      <DialogTitle>Edit {flag.key}</DialogTitle>
      <DialogContent className="flags-editor">
        <TextField
          label="Description"
          value={desc}
          onChange={e => setDesc(e.target.value)}
          fullWidth
        />
        <FormControlLabel
          label="Enabled"
          control={
            <Switch
              checked={enabled}
              onChange={e =>
                setEnabled(e.target.checked)
              }
            />
          }
        />
        <RolloutSlider
          value={pct}
          onChange={setPct}
          disabled={!enabled}
        />
      </DialogContent>
      <DialogActions>
        <Button onClick={onClose}>Cancel</Button>
        <Button
          variant="filled"
          disabled={pending}
          onClick={() => onSave({
            description: desc,
            enabled,
            rollout_pct: pct,
          })}
        >
          Save
        </Button>
      </DialogActions>
    </Dialog>
  )
}
