-- Migration 016: DB-backed session registry for JWT revocation
--
-- user_sessions enables per-JTI revocation so stolen tokens
-- can be invalidated before natural expiry.  Revocation is
-- checked in JwtAuthFilter after signature verification.

CREATE TABLE IF NOT EXISTS user_sessions (
    jti         TEXT PRIMARY KEY,
    user_id     UUID NOT NULL
                    REFERENCES users(id) ON DELETE CASCADE,
    issued_at   TIMESTAMPTZ NOT NULL DEFAULT NOW(),
    expires_at  TIMESTAMPTZ NOT NULL,
    revoked     BOOLEAN NOT NULL DEFAULT FALSE,
    revoked_at  TIMESTAMPTZ,
    user_agent  TEXT NOT NULL DEFAULT '',
    ip_address  TEXT NOT NULL DEFAULT ''
);

CREATE INDEX IF NOT EXISTS idx_user_sessions_user
    ON user_sessions(user_id, issued_at DESC);

CREATE INDEX IF NOT EXISTS idx_user_sessions_expires
    ON user_sessions(expires_at)
    WHERE revoked = FALSE;
