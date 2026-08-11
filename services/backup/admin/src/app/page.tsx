'use client'

/**
 * Backups operator root page — tabbed
 * dashboard over the runs history and
 * policy tables with a manual trigger
 * and a restore confirmation dialog.
 */

import { useState } from 'react'
import { useBackups } from '@/hooks/useBackups'
import { useRestore } from '@/hooks/useRestore'
import { RunsTable } from './RunsTable'
import { PolicyEditor } from './PolicyEditor'
import { RestoreDialog } from './RestoreDialog'

type Tab = 'runs' | 'policy'

export default function BackupsPage() {
  const [tab, setTab] = useState<Tab>('runs')
  const [pending, setPending] =
    useState<string | null>(null)
  const { runs, policies, refresh } =
    useBackups()
  const { trigger, restore } =
    useRestore(refresh)

  return (
    <main className="backups-shell">
      <h1>Backups</h1>
      <div className="backups-toolbar">
        <button
          type="button"
          data-testid="trigger-full"
          aria-label="Trigger full backup"
          onClick={() => trigger('full')}
        >
          Run full backup now
        </button>
        <button
          type="button"
          data-testid="trigger-schema"
          aria-label="Trigger schema backup"
          onClick={() => trigger('schema')}
        >
          Schema only
        </button>
      </div>
      <div
        className="backups-tabs"
        role="tablist"
        aria-label="Backup views"
      >
        <button
          type="button"
          role="tab"
          aria-selected={tab === 'runs'}
          data-testid="tab-runs"
          onClick={() => setTab('runs')}
        >
          Runs ({runs.length})
        </button>
        <button
          type="button"
          role="tab"
          aria-selected={tab === 'policy'}
          data-testid="tab-policy"
          onClick={() => setTab('policy')}
        >
          Policy ({policies.length})
        </button>
      </div>

      {tab === 'runs' && (
        <RunsTable
          rows={runs}
          onRestore={setPending}
        />
      )}
      {tab === 'policy' && (
        <PolicyEditor rows={policies} />
      )}

      <RestoreDialog
        s3Key={pending}
        onConfirm={key => {
          restore(key)
          setPending(null)
        }}
        onCancel={() => setPending(null)}
      />
    </main>
  )
}
