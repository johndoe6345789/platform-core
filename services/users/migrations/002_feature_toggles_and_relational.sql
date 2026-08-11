-- ============================================================
-- Migration 002 : Feature Toggles + Relational Refactor
-- ============================================================
-- Adds feature_toggles for runtime feature management.
-- Replaces JSONB columns with proper relational tables:
--   badge_criteria, notification_metadata.
-- Adds leaderboard view for efficient ranking.
-- ============================================================

BEGIN;

-- ----------------------------------------------------------
-- feature_toggles
-- ----------------------------------------------------------
CREATE TABLE IF NOT EXISTS feature_toggles (
    id          UUID         PRIMARY KEY
                             DEFAULT uuid_generate_v4(),
    key         VARCHAR(100) NOT NULL UNIQUE,
    enabled     BOOLEAN      NOT NULL DEFAULT FALSE,
    description TEXT         NOT NULL DEFAULT '',
    scope       VARCHAR(50)  NOT NULL DEFAULT 'global'
                CHECK (scope IN (
                    'global', 'admin', 'beta'
                )),
    created_at  TIMESTAMPTZ  NOT NULL DEFAULT NOW(),
    updated_at  TIMESTAMPTZ  NOT NULL DEFAULT NOW()
);

CREATE INDEX idx_feature_toggles_key
    ON feature_toggles (key);
CREATE INDEX idx_feature_toggles_scope
    ON feature_toggles (scope);

-- ----------------------------------------------------------
-- badge_criteria (replaces badges.criteria_json)
-- ----------------------------------------------------------
CREATE TABLE IF NOT EXISTS badge_criteria (
    id          UUID         PRIMARY KEY
                             DEFAULT uuid_generate_v4(),
    badge_id    UUID         NOT NULL
                REFERENCES   badges (id) ON DELETE CASCADE,
    metric      VARCHAR(100) NOT NULL,
    operator    VARCHAR(10)  NOT NULL DEFAULT '>='
                CHECK (operator IN (
                    '=', '!=', '>', '>=', '<', '<='
                )),
    threshold   INTEGER      NOT NULL DEFAULT 0,
    sort_order  SMALLINT     NOT NULL DEFAULT 0
);

CREATE INDEX idx_badge_criteria_badge_id
    ON badge_criteria (badge_id);

-- ----------------------------------------------------------
-- notification_links (replaces notification metadata_json)
-- ----------------------------------------------------------
CREATE TABLE IF NOT EXISTS notification_links (
    id              UUID         PRIMARY KEY
                                 DEFAULT uuid_generate_v4(),
    notification_id UUID         NOT NULL
                    REFERENCES   notifications (id)
                    ON DELETE CASCADE,
    rel             VARCHAR(50)  NOT NULL DEFAULT 'related',
    href            TEXT         NOT NULL,

    UNIQUE (notification_id, rel)
);

CREATE INDEX idx_notification_links_nid
    ON notification_links (notification_id);

-- ----------------------------------------------------------
-- Drop JSONB columns now replaced by relational tables
-- ----------------------------------------------------------
ALTER TABLE badges
    DROP COLUMN IF EXISTS criteria_json;
ALTER TABLE notifications
    DROP COLUMN IF EXISTS metadata_json;

-- ----------------------------------------------------------
-- Leaderboard view (efficient ranking query)
-- ----------------------------------------------------------
CREATE OR REPLACE VIEW leaderboard AS
SELECT
    u.id,
    u.username,
    u.display_name,
    u.avatar_url,
    u.total_points,
    u.current_level,
    COALESCE(s.current_streak, 0)  AS current_streak,
    COALESCE(s.longest_streak, 0)  AS longest_streak,
    COUNT(ub.badge_id)             AS badge_count,
    RANK() OVER (
        ORDER BY u.total_points DESC
    )                              AS rank
FROM users u
LEFT JOIN streaks s      ON s.user_id = u.id
LEFT JOIN user_badges ub ON ub.user_id = u.id
WHERE u.is_active = TRUE
GROUP BY u.id, s.current_streak, s.longest_streak;

-- Record this migration
INSERT INTO schema_migrations (filename)
VALUES ('002_feature_toggles_and_relational.sql')
ON CONFLICT (filename) DO NOTHING;

COMMIT;
