'use client'

/**
 * Jobs operator root page — tabbed
 * dashboard over queue, runs, and
 * dead-letter views.  Each tab is its
 * own component so the render stays
 * under the 100-LOC file cap.
 */

import { useState } from 'react'
import { useJobs } from '@/hooks/useJobs'
import {
  useJobActions,
} from '@/hooks/useJobActions'
import { QueueTable } from './QueueTable'
import { RunsTable } from './RunsTable'
import {
  DeadLetterTable,
} from './DeadLetterTable'

type Tab = 'queue' | 'runs' | 'dead'

export default function JobsPage() {
  const [tab, setTab] = useState<Tab>('queue')
  const { queue, runs, dead, refresh } =
    useJobs()
  const { retry, cancel } =
    useJobActions(refresh)

  return (
    <main className="jobs-shell">
      <h1>Job Scheduler</h1>
      <div
        className="jobs-tabs"
        role="tablist"
        aria-label="Job views"
      >
        <button
          type="button"
          role="tab"
          aria-selected={tab === 'queue'}
          data-testid="tab-queue"
          onClick={() => setTab('queue')}
        >
          Queue ({queue.length})
        </button>
        <button
          type="button"
          role="tab"
          aria-selected={tab === 'runs'}
          data-testid="tab-runs"
          onClick={() => setTab('runs')}
        >
          History ({runs.length})
        </button>
        <button
          type="button"
          role="tab"
          aria-selected={tab === 'dead'}
          data-testid="tab-dead"
          onClick={() => setTab('dead')}
        >
          Dead letter ({dead.length})
        </button>
      </div>

      {tab === 'queue' && (
        <QueueTable
          rows={queue}
          onCancel={cancel}
        />
      )}
      {tab === 'runs' && (
        <RunsTable rows={runs} />
      )}
      {tab === 'dead' && (
        <DeadLetterTable
          rows={dead}
          onRetry={retry}
        />
      )}
    </main>
  )
}
