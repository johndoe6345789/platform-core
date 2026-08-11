-- PgAdmin schema: session and user management.
-- Introspection uses information_schema / pg_catalog.

CREATE TABLE IF NOT EXISTS pgadmin_users (
    id          SERIAL PRIMARY KEY,
    username    TEXT UNIQUE NOT NULL,
    pass_hash   TEXT NOT NULL,
    role        TEXT NOT NULL DEFAULT 'admin',
    created_at  TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE TABLE IF NOT EXISTS pgadmin_sessions (
    id          TEXT PRIMARY KEY,
    user_id     INTEGER NOT NULL
                REFERENCES pgadmin_users(id)
                ON DELETE CASCADE,
    created_at  TIMESTAMPTZ NOT NULL DEFAULT now(),
    expires_at  TIMESTAMPTZ NOT NULL
);

CREATE INDEX IF NOT EXISTS idx_sessions_expires
    ON pgadmin_sessions (expires_at);
