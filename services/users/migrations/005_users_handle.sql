-- ============================================================
-- Migration 005 : Add handle column to users
-- ============================================================
-- Adds a unique @handle for public profile lookups.
-- Derives initial handles from display_name if absent.
-- ============================================================

BEGIN;

ALTER TABLE users
    ADD COLUMN IF NOT EXISTS handle TEXT UNIQUE;

UPDATE users
SET handle = lower(
    regexp_replace(
        display_name,
        '[^a-zA-Z0-9]',
        '_',
        'g'))
WHERE handle IS NULL;

COMMIT;
