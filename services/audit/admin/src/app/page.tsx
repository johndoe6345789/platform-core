'use client'

/**
 * @file page.tsx
 * @brief Audit operator dashboard — search
 *        by tenant + verify hash chain.
 */

import { useState } from 'react'
import {
  Button, Input,
} from '@shared/m3'

import { useAudit } from '@/hooks/useAudit'
import { useIntegrity }
  from '@/hooks/useIntegrity'
import EventTable from './EventTable'
import IntegrityBadge from './IntegrityBadge'

const DEFAULT_TENANT =
  '00000000-0000-0000-0000-000000000000'

/** Audit dashboard page component. */
export default function AuditPage() {
  const [tenant, setTenant] =
    useState(DEFAULT_TENANT)
  const { events, loading, error, refresh } =
    useAudit({ tenant, limit: 100 })
  const {
    report, loading: verifying, verify,
  } = useIntegrity(tenant)

  return (
    <div className="audit-shell">
      <header className="audit-header">
        <div className="audit-title">
          <span className={
            'material-symbols-outlined'
          }>
            verified_user
          </span>
          <h1 style={{ margin: 0 }}>
            Audit
          </h1>
        </div>
        <IntegrityBadge
          report={report}
          loading={verifying}
        />
      </header>

      <div
        style={{
          display: 'flex',
          gap: 8,
          marginBottom: 16,
        }}
      >
        <Input
          value={tenant}
          onChange={(e) =>
            setTenant(e.target.value)
          }
          placeholder="Tenant UUID"
          data-testid="audit-tenant-input"
          aria-label="tenant UUID"
        />
        <Button
          onClick={() => refresh()}
          data-testid="audit-refresh"
          aria-label="refresh events"
        >
          Refresh
        </Button>
        <Button
          onClick={() => verify()}
          data-testid="audit-verify"
          aria-label="verify hash chain"
        >
          Verify
        </Button>
      </div>

      {error && (
        <div className="audit-empty">
          {error}
        </div>
      )}
      {loading ? (
        <div className="audit-empty">
          Loading…
        </div>
      ) : (
        <EventTable events={events} />
      )}
    </div>
  )
}
