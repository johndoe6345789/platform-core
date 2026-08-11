-- Add preferred AI provider to user preferences.
ALTER TABLE users
    ADD COLUMN IF NOT EXISTS ai_provider
        VARCHAR(20) NOT NULL DEFAULT 'claude'
        CHECK (ai_provider IN (
            'claude', 'openai'));
