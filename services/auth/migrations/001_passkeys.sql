-- Migration 013: WebAuthn passkey credentials
--
-- Stores registered passkey credentials for users.  A user may
-- have multiple passkeys (one per device).  sign_count is used
-- for cloning detection per the WebAuthn spec.

CREATE TABLE IF NOT EXISTS passkey_credentials (
    id              bigserial PRIMARY KEY,
    user_id         uuid NOT NULL
                        REFERENCES users(id) ON DELETE CASCADE,
    credential_id   bytea NOT NULL UNIQUE,
    public_key      bytea NOT NULL,
    sign_count      bigint NOT NULL DEFAULT 0,
    transports      text[] NOT NULL DEFAULT '{}',
    aaguid          bytea,
    nickname        text,
    created_at      timestamptz NOT NULL DEFAULT now(),
    last_used_at    timestamptz
);

CREATE INDEX IF NOT EXISTS idx_passkey_user
    ON passkey_credentials(user_id);
