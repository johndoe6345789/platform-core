-- Migration 013: Feature flags
--
-- Feature flags extend feature_toggles (migration 002) with
-- rollout percentage + targeting rules + an audit trail. Flags
-- are low-traffic config lookups served from the main backend
-- daemon. Evaluation is deterministic on (key, user_id).

CREATE TABLE IF NOT EXISTS feature_flags (
    id            BIGSERIAL PRIMARY KEY,
    key           TEXT UNIQUE NOT NULL,
    description   TEXT NOT NULL DEFAULT '',
    enabled       BOOLEAN NOT NULL DEFAULT FALSE,
    rollout_pct   SMALLINT NOT NULL DEFAULT 0
                  CHECK (rollout_pct BETWEEN 0 AND 100),
    targeting     JSONB NOT NULL DEFAULT '{}'::jsonb,
    updated_at    TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS feature_flags_key_idx
    ON feature_flags (key);

CREATE TABLE IF NOT EXISTS feature_flag_audit (
    id        BIGSERIAL PRIMARY KEY,
    flag_id   BIGINT NOT NULL
              REFERENCES feature_flags(id)
              ON DELETE CASCADE,
    actor_id  UUID,
    prev      JSONB NOT NULL DEFAULT '{}'::jsonb,
    next      JSONB NOT NULL DEFAULT '{}'::jsonb,
    at        TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS feature_flag_audit_flag_idx
    ON feature_flag_audit (flag_id, at DESC);

INSERT INTO feature_flags
    (key, description, enabled, rollout_pct, targeting)
VALUES
    ('auth.passkeys',
     'WebAuthn passkey login',
     FALSE, 0, '{}'::jsonb),
    ('ui.darkMode',
     'Dark mode theme toggle',
     TRUE, 100, '{}'::jsonb),
    ('experimental.aiChat',
     'Next-gen AI chat experience',
     FALSE, 10, '{}'::jsonb)
ON CONFLICT (key) DO NOTHING;
