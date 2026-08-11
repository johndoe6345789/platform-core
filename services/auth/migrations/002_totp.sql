-- Migration 014: TOTP 2FA enrolment
--
-- Per-user TOTP state.  secret_b32 is the shared secret in
-- RFC 4648 Base32.  recovery_codes stores SHA-256 hex hashes
-- (plaintext is shown to the user once at enrolment time).

CREATE TABLE IF NOT EXISTS user_totp (
    user_id         uuid PRIMARY KEY
                        REFERENCES users(id) ON DELETE CASCADE,
    secret_b32      text NOT NULL,
    enabled         boolean NOT NULL DEFAULT false,
    enrolled_at     timestamptz,
    recovery_codes  text[] NOT NULL DEFAULT '{}'
);
