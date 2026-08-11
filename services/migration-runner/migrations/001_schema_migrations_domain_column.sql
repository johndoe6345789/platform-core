-- Bootstrap migration for domain-sliced refactor.
-- Adds a `domain` column to schema_migrations so each
-- domain tracks its own numbering from 001. Existing rows
-- are stamped as domain='legacy' so old filenames stay
-- unique under the new composite primary key.

ALTER TABLE IF EXISTS schema_migrations
    ADD COLUMN IF NOT EXISTS domain TEXT NOT NULL DEFAULT 'legacy';

-- Drop old PK and create composite PK (domain, filename).
-- Using DO block so the migration is idempotent if it's
-- re-applied after a partial failure.
DO $$
BEGIN
    IF EXISTS (
        SELECT 1 FROM pg_constraint
        WHERE conname = 'schema_migrations_pkey'
    ) THEN
        ALTER TABLE schema_migrations
            DROP CONSTRAINT schema_migrations_pkey;
    END IF;
    IF NOT EXISTS (
        SELECT 1 FROM pg_constraint
        WHERE conname = 'schema_migrations_domain_filename_pkey'
    ) THEN
        ALTER TABLE schema_migrations
            ADD CONSTRAINT schema_migrations_domain_filename_pkey
            PRIMARY KEY (domain, filename);
    END IF;
END $$;
