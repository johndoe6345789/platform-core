-- ============================================================
-- Migration 009 : API Key Storage
-- ============================================================
-- Adds per-user API keys and system-wide settings.
-- Key resolution order: user → system → environment.
-- ============================================================

BEGIN;

-- ----------------------------------------------------------
-- user_api_keys  (per-user provider credentials)
-- ----------------------------------------------------------
CREATE TABLE IF NOT EXISTS user_api_keys (
    id          UUID         PRIMARY KEY
                             DEFAULT uuid_generate_v4(),
    user_id     UUID         NOT NULL
                REFERENCES   users (id) ON DELETE CASCADE,
    provider    VARCHAR(50)  NOT NULL,
    api_key     TEXT         NOT NULL,
    model       VARCHAR(100) NOT NULL DEFAULT '',
    created_at  TIMESTAMPTZ  NOT NULL DEFAULT NOW(),
    updated_at  TIMESTAMPTZ  NOT NULL DEFAULT NOW(),

    UNIQUE (user_id, provider)
);

CREATE INDEX idx_user_api_keys_user_id
    ON user_api_keys (user_id);

-- ----------------------------------------------------------
-- system_settings  (admin-managed key/value pairs)
-- ----------------------------------------------------------
CREATE TABLE IF NOT EXISTS system_settings (
    key         VARCHAR(100) PRIMARY KEY,
    value       TEXT         NOT NULL DEFAULT '',
    updated_at  TIMESTAMPTZ  NOT NULL DEFAULT NOW()
);

-- Record this migration
INSERT INTO schema_migrations (filename)
VALUES ('009_api_keys.sql')
ON CONFLICT (filename) DO NOTHING;

COMMIT;
