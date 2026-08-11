'use client'

/**
 * Public status page — composes the live service
 * grid, the historical uptime bar, and the
 * incident feed. Intentionally public (no SSO).
 */

import { Typography } from '@shared/m3'
import { StatusGrid } from './StatusGrid'
import { IncidentList } from './IncidentList'
import { UptimeBar } from './UptimeBar'

export default function StatusPage() {
  return (
    <main
      className="status-shell"
      data-testid="status-shell"
    >
      <header>
        <Typography
          variant="h1"
          testId="status-heading"
        >
          Service status
        </Typography>
        <Typography variant="body1">
          Live health of every Nextra service.
          This page updates every 15 seconds.
        </Typography>
      </header>

      <section aria-label="Service grid">
        <StatusGrid />
      </section>

      <section aria-label="Uptime history">
        <Typography variant="h2">
          Uptime — last 90 days
        </Typography>
        <UptimeBar />
      </section>

      <section aria-label="Incidents">
        <Typography variant="h2">
          Incidents
        </Typography>
        <IncidentList />
      </section>
    </main>
  )
}
