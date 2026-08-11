'use client'

/**
 * IncidentList — renders active incidents and a
 * short history tail. Driven by useIncidents.
 */

import {
  Alert,
  Card,
  Chip,
  Typography,
} from '@shared/m3'
import {
  useIncidents,
  type Incident,
} from '@/hooks/useIncidents'

function severityColor(
  sev: Incident['severity'],
): 'info' | 'warning' | 'error' {
  if (sev === 'critical') return 'error'
  if (sev === 'major') return 'warning'
  return 'info'
}

function IncidentRow({ inc }: { inc: Incident }) {
  return (
    <Card
      style={{ padding: 16 }}
      data-testid={`incident-${inc.id}`}
    >
      <Typography variant="h3">
        {inc.title}
      </Typography>
      <div style={{ marginTop: 8 }}>
        <Chip
          label={inc.severity}
          color={severityColor(inc.severity)}
        />
        <span style={{ marginLeft: 8 }}>
          <Chip label={inc.status} />
        </span>
      </div>
      {inc.body && (
        <Typography
          variant="body2"
          style={{ marginTop: 8 }}
        >
          {inc.body}
        </Typography>
      )}
    </Card>
  )
}

export function IncidentList() {
  const { active, history, error } =
    useIncidents()
  if (error) {
    return (
      <Alert
        severity="warning"
        title="Incident feed unavailable"
        testId="incidents-error"
      >
        {error}
      </Alert>
    )
  }
  const rows =
    active.length > 0
      ? active
      : history.slice(0, 5)
  if (rows.length === 0) {
    return (
      <Typography variant="body1">
        No incidents reported.
      </Typography>
    )
  }
  return (
    <div
      data-testid="incident-list"
      aria-label="Incident list"
    >
      {rows.map((inc) => (
        <IncidentRow key={inc.id} inc={inc} />
      ))}
    </div>
  )
}
