'use client'

/**
 * Cron operator root page — lists every
 * row in scheduled_jobs with toggle,
 * fire-now, and expression-edit affordances.
 * The live preview for the expression
 * editor lives in ExpressionEditor.
 */

import { useCron } from '@/hooks/useCron'
import {
  ScheduleTable,
} from './ScheduleTable'
import {
  ExpressionEditor,
} from './ExpressionEditor'

export default function CronPage() {
  const { items, toggle, fireNow, refresh } =
    useCron()

  return (
    <main className="cron-shell">
      <h1>Cron Manager</h1>
      <ExpressionEditor
        onCreated={refresh}
      />
      <ScheduleTable
        rows={items}
        onToggle={toggle}
        onFireNow={fireNow}
      />
    </main>
  )
}
