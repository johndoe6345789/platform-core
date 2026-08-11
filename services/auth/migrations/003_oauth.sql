-- Migration 015: OAuth social identities and CSRF state
--
-- oauth_identities maps a third-party identity to a local
-- user account.  oauth_states stores pending authorize
-- requests for CSRF/PKCE protection.

CREATE TABLE IF NOT EXISTS oauth_identities (
    id              bigserial PRIMARY KEY,
    user_id         uuid NOT NULL
                        REFERENCES users(id) ON DELETE CASCADE,
    provider        text NOT NULL,
    subject         text NOT NULL,
    email           text,
    avatar_url      text,
    linked_at       timestamptz NOT NULL DEFAULT now(),
    UNIQUE(provider, subject)
);

CREATE INDEX IF NOT EXISTS idx_oauth_user
    ON oauth_identities(user_id);

CREATE TABLE IF NOT EXISTS oauth_states (
    state           text PRIMARY KEY,
    nonce           text NOT NULL,
    redirect_uri    text NOT NULL,
    provider        text NOT NULL,
    code_verifier   text NOT NULL,
    created_at      timestamptz NOT NULL DEFAULT now(),
    consumed_at     timestamptz
);
