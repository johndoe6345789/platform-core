-- 013_incidents.sql
-- Public status page: incidents table.
--
-- An incident is a human-authored record of degraded or
-- broken service. The /status tool renders open incidents
-- on its landing page and the history endpoint for past
-- events.

CREATE TABLE IF NOT EXISTS incidents (
    id          BIGSERIAL PRIMARY KEY,
    title       TEXT NOT NULL,
    body        TEXT NOT NULL DEFAULT '',
    severity    TEXT NOT NULL
                CHECK (severity IN (
                    'minor', 'major', 'critical')),
    status      TEXT NOT NULL
                CHECK (status IN (
                    'investigating',
                    'identified',
                    'monitoring',
                    'resolved')),
    started_at  TIMESTAMPTZ NOT NULL DEFAULT now(),
    resolved_at TIMESTAMPTZ,
    created_at  TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS incidents_status_idx
    ON incidents (status);

CREATE INDEX IF NOT EXISTS incidents_started_at_idx
    ON incidents (started_at DESC);
