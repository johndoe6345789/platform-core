-- Default admin user.  Password: admin
-- bcrypt hash generated with cost 10.
INSERT INTO pgadmin_users (username, pass_hash)
VALUES (
    'admin',
    '$2a$10$N9qo8uLOickgx2ZMRZoMye'
    'IjZAgcfl7p92ldGxad68LJZdL17lhWy'
)
ON CONFLICT (username) DO NOTHING;
