'use client'

/**
 * ServiceCard — M3 Card wrapper around a single
 * ServiceStatus row. Colours the chip by liveness.
 */

import { Card, Chip, Typography } from '@shared/m3'
import type { ServiceStatus } from '@/hooks/useStatus'

interface Props {
  service: ServiceStatus
}

export function ServiceCard({ service }: Props) {
  const label = service.ok ? 'Operational' : 'Down'
  const color = service.ok ? 'success' : 'error'
  return (
    <Card
      data-testid={`service-${service.name}`}
      aria-label={
        `Service ${service.name} ${label}`
      }
      style={{ padding: 16 }}
    >
      <Typography variant="h3">
        {service.name}
      </Typography>
      <div style={{ marginTop: 8 }}>
        <Chip
          label={label}
          color={color}
          data-testid={
            `chip-${service.name}`
          }
        />
      </div>
      {typeof service.latencyMs === 'number' && (
        <Typography
          variant="caption"
          style={{ marginTop: 8 }}
        >
          {`${service.latencyMs} ms`}
        </Typography>
      )}
      {service.message && (
        <Typography
          variant="body2"
          style={{ marginTop: 4 }}
        >
          {service.message}
        </Typography>
      )}
    </Card>
  )
}
