-- Migration 013: Hash-chained audit log (Phase 1.2)
--
-- The audit daemon consumes the Kafka topic `audit.events` and
-- appends one row per event to this table.  It is partitioned by
-- month on created_at so that the janitor job can drop old
-- partitions after archiving them to s3.
--
-- Tamper evidence: each row's hash = SHA256(prev_hash ||
-- canonical_json(payload)).  prev_hash is the hash of the previous
-- row for the same tenant_id (NULL seed for the first row).  The
-- /api/audit/verify endpoint walks the chain and reports the first
-- divergence.  Daily Merkle roots are written to audit_checkpoints.

CREATE TABLE IF NOT EXISTS audit_events (
    id           BIGSERIAL,
    tenant_id    UUID        NOT NULL,
    actor_id     UUID,
    action       TEXT        NOT NULL,
    target_type  TEXT,
    target_id    TEXT,
    payload      JSONB       NOT NULL DEFAULT '{}'::jsonb,
    prev_hash    BYTEA,
    hash         BYTEA       NOT NULL,
    created_at   TIMESTAMPTZ NOT NULL DEFAULT now(),
    PRIMARY KEY (id, created_at)
) PARTITION BY RANGE (created_at);

CREATE INDEX IF NOT EXISTS audit_events_tenant_time_idx
    ON audit_events (tenant_id, created_at DESC);

CREATE INDEX IF NOT EXISTS audit_events_action_idx
    ON audit_events (action);

-- Monthly partitions for 2026
CREATE TABLE IF NOT EXISTS audit_events_2026_01 PARTITION OF audit_events
    FOR VALUES FROM ('2026-01-01') TO ('2026-02-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_02 PARTITION OF audit_events
    FOR VALUES FROM ('2026-02-01') TO ('2026-03-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_03 PARTITION OF audit_events
    FOR VALUES FROM ('2026-03-01') TO ('2026-04-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_04 PARTITION OF audit_events
    FOR VALUES FROM ('2026-04-01') TO ('2026-05-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_05 PARTITION OF audit_events
    FOR VALUES FROM ('2026-05-01') TO ('2026-06-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_06 PARTITION OF audit_events
    FOR VALUES FROM ('2026-06-01') TO ('2026-07-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_07 PARTITION OF audit_events
    FOR VALUES FROM ('2026-07-01') TO ('2026-08-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_08 PARTITION OF audit_events
    FOR VALUES FROM ('2026-08-01') TO ('2026-09-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_09 PARTITION OF audit_events
    FOR VALUES FROM ('2026-09-01') TO ('2026-10-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_10 PARTITION OF audit_events
    FOR VALUES FROM ('2026-10-01') TO ('2026-11-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_11 PARTITION OF audit_events
    FOR VALUES FROM ('2026-11-01') TO ('2026-12-01');
CREATE TABLE IF NOT EXISTS audit_events_2026_12 PARTITION OF audit_events
    FOR VALUES FROM ('2026-12-01') TO ('2027-01-01');

-- Daily Merkle checkpoints (never pruned — used by the verifier
-- to anchor the chain across archive boundaries).
CREATE TABLE IF NOT EXISTS audit_checkpoints (
    id          BIGSERIAL PRIMARY KEY,
    tenant_id   UUID        NOT NULL,
    day         DATE        NOT NULL,
    merkle_root BYTEA       NOT NULL,
    row_count   BIGINT      NOT NULL,
    created_at  TIMESTAMPTZ NOT NULL DEFAULT now(),
    UNIQUE (tenant_id, day)
);
