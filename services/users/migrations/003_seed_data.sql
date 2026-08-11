-- ============================================================
-- Migration 003 : Seed Data
-- ============================================================
-- Inserts development seed data into all core tables.
-- All data is relational — no JSON blobs stored.
-- Password hash is bcrypt of "nextra_dev".
-- ============================================================

BEGIN;

-- ----------------------------------------------------------
-- Seed users
-- ----------------------------------------------------------
INSERT INTO users
    (email, username, display_name, role,
     is_active, email_confirmed,
     total_points, current_level, password_hash)
VALUES
    -- admin / Admin123!
    ('admin@nextra.local', 'admin', 'Admin User',
     'admin', TRUE, TRUE, 500, 8,
     'd93aef3e5b36973a841b911c78d92f1a'
     '$600000$'
     '8dbc2ae5c2bcde0cdae9d40c16805dc3'
     'cdfd3b85b0b7c8ba192f0ca464b983aa'),
    -- testuser / User123!
    ('user@nextra.local', 'testuser', 'Test User',
     'user', TRUE, TRUE, 150, 3,
     '1eca5055645a490144b007c9aa9ee0a7'
     '$600000$'
     'e34eb0cc26a04be77f57d6120fdf6a54'
     '6641d524c1f318981226f129a4f050cc'),
    ('noreply@nextra.local', 'system', 'System',
     'admin', TRUE, TRUE, 0, 1,
     'd93aef3e5b36973a841b911c78d92f1a'
     '$600000$'
     '8dbc2ae5c2bcde0cdae9d40c16805dc3'
     'cdfd3b85b0b7c8ba192f0ca464b983aa')
ON CONFLICT (email) DO NOTHING;

-- ----------------------------------------------------------
-- Seed streaks
-- ----------------------------------------------------------
INSERT INTO streaks (user_id, current_streak,
    longest_streak, last_activity_date)
SELECT id, 12, 30, CURRENT_DATE
FROM users WHERE username = 'admin'
ON CONFLICT (user_id) DO NOTHING;

INSERT INTO streaks (user_id, current_streak,
    longest_streak, last_activity_date)
SELECT id, 5, 14, CURRENT_DATE
FROM users WHERE username = 'testuser'
ON CONFLICT (user_id) DO NOTHING;

-- ----------------------------------------------------------
-- Seed badges
-- ----------------------------------------------------------
INSERT INTO badges (name, description, category,
    icon_url, points_required)
VALUES
    ('First Login',
     'Logged in for the first time',
     'milestone', '/badges/first-login.svg', 0),
    ('Streak Starter',
     'Maintained a 3-day login streak',
     'streak', '/badges/streak-3.svg', 0),
    ('Streak Master',
     'Maintained a 7-day login streak',
     'streak', '/badges/streak-7.svg', 0),
    ('Point Collector',
     'Earned 100 total points',
     'points', '/badges/points-100.svg', 100),
    ('Rising Star',
     'Reached level 5',
     'level', '/badges/level-5.svg', 0),
    ('Chat Explorer',
     'Sent 10 messages in AI chat',
     'engagement', '/badges/chat-10.svg', 0)
ON CONFLICT (name) DO NOTHING;

-- ----------------------------------------------------------
-- Seed badge_criteria (relational, not JSON)
-- ----------------------------------------------------------
INSERT INTO badge_criteria
    (badge_id, metric, operator, threshold, sort_order)
SELECT b.id, 'login_count', '>=', 1, 0
FROM badges b WHERE b.name = 'First Login'
ON CONFLICT DO NOTHING;

INSERT INTO badge_criteria
    (badge_id, metric, operator, threshold, sort_order)
SELECT b.id, 'current_streak', '>=', 3, 0
FROM badges b WHERE b.name = 'Streak Starter'
ON CONFLICT DO NOTHING;

INSERT INTO badge_criteria
    (badge_id, metric, operator, threshold, sort_order)
SELECT b.id, 'current_streak', '>=', 7, 0
FROM badges b WHERE b.name = 'Streak Master'
ON CONFLICT DO NOTHING;

INSERT INTO badge_criteria
    (badge_id, metric, operator, threshold, sort_order)
SELECT b.id, 'total_points', '>=', 100, 0
FROM badges b WHERE b.name = 'Point Collector'
ON CONFLICT DO NOTHING;

INSERT INTO badge_criteria
    (badge_id, metric, operator, threshold, sort_order)
SELECT b.id, 'current_level', '>=', 5, 0
FROM badges b WHERE b.name = 'Rising Star'
ON CONFLICT DO NOTHING;

INSERT INTO badge_criteria
    (badge_id, metric, operator, threshold, sort_order)
SELECT b.id, 'chat_message_count', '>=', 10, 0
FROM badges b WHERE b.name = 'Chat Explorer'
ON CONFLICT DO NOTHING;

-- ----------------------------------------------------------
-- Seed user_badges (admin has all, testuser has 3)
-- ----------------------------------------------------------
INSERT INTO user_badges (user_id, badge_id)
SELECT u.id, b.id FROM users u, badges b
WHERE u.username = 'admin'
ON CONFLICT (user_id, badge_id) DO NOTHING;

INSERT INTO user_badges (user_id, badge_id)
SELECT u.id, b.id FROM users u, badges b
WHERE u.username = 'testuser'
  AND b.name IN (
      'First Login', 'Streak Starter', 'Point Collector'
  )
ON CONFLICT (user_id, badge_id) DO NOTHING;

-- ----------------------------------------------------------
-- Seed points_log
-- ----------------------------------------------------------
INSERT INTO points_log
    (user_id, amount, reason, source)
SELECT id, 50, 'Welcome bonus', 'system'
FROM users WHERE username = 'testuser';

INSERT INTO points_log
    (user_id, amount, reason, source)
SELECT id, 100, 'Completed tutorial', 'tutorial'
FROM users WHERE username = 'testuser';

INSERT INTO points_log
    (user_id, amount, reason, source)
SELECT id, 500, 'Platform setup', 'system'
FROM users WHERE username = 'admin';

-- ----------------------------------------------------------
-- Seed notifications
-- ----------------------------------------------------------
INSERT INTO notifications
    (user_id, title, body, type, is_read)
SELECT id,
    'Welcome to NextExtra!',
    'Start exploring the platform to earn badges.',
    'info', FALSE
FROM users WHERE username = 'testuser';

INSERT INTO notifications
    (user_id, title, body, type, is_read)
SELECT id,
    'Badge Earned: First Login',
    'Congratulations! You earned the First Login badge.',
    'achievement', FALSE
FROM users WHERE username = 'testuser';

INSERT INTO notifications
    (user_id, title, body, type, is_read)
SELECT id,
    'New streak milestone',
    'You have a 5-day login streak! Keep it going.',
    'streak', TRUE
FROM users WHERE username = 'testuser';

INSERT INTO notifications
    (user_id, title, body, type, is_read)
SELECT id,
    'System update deployed',
    'Version 1.1 is now live with feature toggles.',
    'system', FALSE
FROM users WHERE username = 'admin';

-- ----------------------------------------------------------
-- Seed notification_links
-- ----------------------------------------------------------
INSERT INTO notification_links
    (notification_id, rel, href)
SELECT n.id, 'action', '/dashboard'
FROM notifications n
JOIN users u ON u.id = n.user_id
WHERE u.username = 'testuser'
  AND n.title = 'Welcome to NextExtra!';

INSERT INTO notification_links
    (notification_id, rel, href)
SELECT n.id, 'badge', '/profile'
FROM notifications n
JOIN users u ON u.id = n.user_id
WHERE u.username = 'testuser'
  AND n.title = 'Badge Earned: First Login';

-- ----------------------------------------------------------
-- Seed feature_toggles
-- ----------------------------------------------------------
INSERT INTO feature_toggles
    (key, enabled, description, scope)
VALUES
    ('ai_chat', TRUE,
     'AI chat integration with Claude and OpenAI',
     'global'),
    ('gamification', TRUE,
     'Points, badges, streaks, and leaderboard',
     'global'),
    ('notifications', TRUE,
     'Real-time notification system',
     'global'),
    ('dark_mode', TRUE,
     'Dark and light theme toggle',
     'global'),
    ('i18n', TRUE,
     'Multi-language support (en, es, fr, de)',
     'global'),
    ('contact_form', TRUE,
     'Public contact form with email delivery',
     'global'),
    ('password_reset', TRUE,
     'Forgot password email flow',
     'global'),
    ('admin_panel', FALSE,
     'Admin dashboard for user management',
     'admin'),
    ('beta_features', FALSE,
     'Experimental features for beta testers',
     'beta')
ON CONFLICT (key) DO NOTHING;

-- Record this migration
INSERT INTO schema_migrations (filename)
VALUES ('003_seed_data.sql')
ON CONFLICT (filename) DO NOTHING;

COMMIT;
