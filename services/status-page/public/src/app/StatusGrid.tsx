'use client'

/**
 * StatusGrid — renders one ServiceCard per entry
 * returned by useStatus. Handles loading and
 * error states via M3 components (no raw HTML).
 */

import {
  Alert,
  LinearProgress,
  Typography,
} from '@shared/m3'
import { useStatus } from '@/hooks/useStatus'
import { ServiceCard } from './ServiceCard'

export function StatusGrid() {
  const { services, loading, error } = useStatus()

  if (loading) {
    return (
      <div data-testid="status-loading">
        <LinearProgress />
      </div>
    )
  }
  if (error) {
    return (
      <Alert
        severity="error"
        title="Status unavailable"
        testId="status-error"
      >
        {error}
      </Alert>
    )
  }
  if (services.length === 0) {
    return (
      <Typography
        variant="body1"
        data-testid="status-empty"
      >
        No services reporting yet.
      </Typography>
    )
  }
  return (
    <div
      className="status-grid"
      data-testid="status-grid"
      aria-label="Service health grid"
    >
      {services.map((s) => (
        <ServiceCard
          key={s.name}
          service={s}
        />
      ))}
    </div>
  )
}
