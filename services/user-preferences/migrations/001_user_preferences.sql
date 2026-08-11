-- Add user preference columns for theme and locale.
ALTER TABLE users
    ADD COLUMN theme_mode VARCHAR(10)
        NOT NULL DEFAULT 'system'
        CHECK (theme_mode IN ('light', 'dark', 'system')),
    ADD COLUMN preferred_locale VARCHAR(10)
        NOT NULL DEFAULT 'en';
