'use client'

/**
 * Expression editor — lets operators type
 * a cron spec and see the next five fire
 * times live before committing a new
 * schedule row.  Posts to /api/cron/
 * schedules on save.
 */

import { useState } from 'react'
import { useCronPreview } from '@/hooks/useCronPreview'
import { CronPreviewPanel } from './CronPreviewPanel'

interface Props {
  /** Called after a schedule is successfully saved. */
  onCreated: () => void
}

/** @param props - Editor callbacks. */
export function ExpressionEditor({ onCreated }: Props) {
  const [name, setName] = useState('')
  const [cron, setCron] = useState('')
  const [handler, setHandler] = useState('')
  const { fires, error } = useCronPreview(cron)

  const canSave = name && cron && handler && !error

  const save = async () => {
    await fetch('/api/cron/schedules', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name, cron, handler }),
    })
    setName('')
    setCron('')
    setHandler('')
    onCreated()
  }

  return (
    <section
      aria-label="New schedule"
      data-testid="expression-editor"
    >
      <h2>New schedule</h2>
      <input
        type="text"
        placeholder="name"
        aria-label="schedule name"
        value={name}
        onChange={e => setName(e.target.value)}
      />
      <input
        type="text"
        placeholder="cron expression"
        aria-label="cron expression"
        value={cron}
        onChange={e => setCron(e.target.value)}
      />
      <input
        type="text"
        placeholder="handler"
        aria-label="handler name"
        value={handler}
        onChange={e => setHandler(e.target.value)}
      />
      <button
        type="button"
        disabled={!canSave}
        onClick={save}
      >
        Save
      </button>
      <CronPreviewPanel fires={fires} error={error} />
    </section>
  )
}
