-- Migration 012: Durable job scheduler
--
-- Provides cron-style scheduled jobs, an ad-hoc priority queue with
-- exponential backoff, run history, and a dead-letter table.  Every
-- later daemon (audit pruner, backups, search reindex, webhook retry,
-- notification digest, stream recording cleanup) dispatches its
-- background work through this scheduler instead of rolling its own
-- cron.  Workers use SELECT ... FOR UPDATE SKIP LOCKED to make the
-- queue safe across multiple concurrent worker processes.

CREATE TABLE scheduled_jobs (
    id SERIAL PRIMARY KEY,
    name TEXT UNIQUE NOT NULL,
    cron TEXT NOT NULL,
    handler TEXT NOT NULL,
    payload JSONB NOT NULL DEFAULT '{}'::jsonb,
    enabled BOOLEAN NOT NULL DEFAULT TRUE,
    next_run_at TIMESTAMPTZ,
    last_run_at TIMESTAMPTZ,
    description TEXT,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    updated_at TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE job_queue (
    id BIGSERIAL PRIMARY KEY,
    name TEXT NOT NULL,
    handler TEXT NOT NULL,
    payload JSONB NOT NULL DEFAULT '{}'::jsonb,
    priority INT NOT NULL DEFAULT 100,
    run_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    attempts INT NOT NULL DEFAULT 0,
    max_attempts INT NOT NULL DEFAULT 5,
    backoff_strategy TEXT NOT NULL DEFAULT 'exponential',
    status TEXT NOT NULL DEFAULT 'queued',
    locked_by TEXT,
    locked_at TIMESTAMPTZ,
    scheduled_job_id INT REFERENCES scheduled_jobs(id) ON DELETE SET NULL,
    created_at TIMESTAMPTZ NOT NULL DEFAULT now(),
    CHECK (status IN ('queued','running','retrying','succeeded','failed'))
);
CREATE INDEX idx_job_queue_ready
    ON job_queue (run_at, priority)
    WHERE status IN ('queued','retrying');
CREATE INDEX idx_job_queue_sched ON job_queue (scheduled_job_id);

CREATE TABLE job_runs (
    id BIGSERIAL PRIMARY KEY,
    job_queue_id BIGINT,
    name TEXT NOT NULL,
    handler TEXT NOT NULL,
    attempt INT NOT NULL,
    status TEXT NOT NULL,
    started_at TIMESTAMPTZ NOT NULL,
    finished_at TIMESTAMPTZ,
    duration_ms INT,
    error TEXT,
    result JSONB,
    worker_id TEXT
);
CREATE INDEX idx_job_runs_name ON job_runs (name, started_at DESC);
CREATE INDEX idx_job_runs_status ON job_runs (status, started_at DESC);

CREATE TABLE job_dead_letter (
    id BIGSERIAL PRIMARY KEY,
    original_id BIGINT NOT NULL,
    name TEXT NOT NULL,
    handler TEXT NOT NULL,
    payload JSONB NOT NULL,
    attempts INT NOT NULL,
    last_error TEXT,
    failed_at TIMESTAMPTZ NOT NULL DEFAULT now()
);
CREATE INDEX idx_job_dead_failed_at ON job_dead_letter (failed_at DESC);
