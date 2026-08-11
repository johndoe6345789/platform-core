-- Migration 013: Backup manager
--
-- Tracks runs of the nightly pg_dump backup daemon and the
-- retention policies that drive it.  The backup-manager
-- daemon inserts one backup_runs row per execution and
-- streams the encrypted dump to the in-repo s3server.  The
-- /backups operator tool reads both tables to render its
-- dashboard and trigger restore flows.

CREATE TABLE backup_runs (
    id            BIGSERIAL PRIMARY KEY,
    started_at    TIMESTAMPTZ NOT NULL DEFAULT now(),
    finished_at   TIMESTAMPTZ,
    status        TEXT NOT NULL DEFAULT 'running',
    bytes         BIGINT NOT NULL DEFAULT 0,
    s3_key        TEXT,
    error         TEXT,
    kind          TEXT NOT NULL
                  CHECK (kind IN ('full','schema','data'))
);

CREATE INDEX backup_runs_started_idx
    ON backup_runs (started_at DESC);

CREATE TABLE backup_policy (
    id            BIGSERIAL PRIMARY KEY,
    name          TEXT UNIQUE NOT NULL,
    kind          TEXT NOT NULL
                  CHECK (kind IN ('full','schema','data')),
    retain_days   INT NOT NULL DEFAULT 30,
    schedule_cron TEXT NOT NULL DEFAULT '@daily'
);

INSERT INTO backup_policy (name, kind, retain_days, schedule_cron)
VALUES ('default', 'full', 30, '@daily')
ON CONFLICT (name) DO NOTHING;
