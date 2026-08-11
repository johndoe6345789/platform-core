-- Create translations table for i18n.
-- Each row stores one key-value pair for a
-- specific locale and namespace.

CREATE TABLE translations (
    id         UUID PRIMARY KEY
                   DEFAULT gen_random_uuid(),
    locale     VARCHAR(10)  NOT NULL,
    namespace  VARCHAR(50)  NOT NULL,
    key        VARCHAR(100) NOT NULL,
    value      TEXT         NOT NULL,
    updated_by UUID         REFERENCES users(id)
                            ON DELETE SET NULL,
    created_at TIMESTAMPTZ  NOT NULL DEFAULT NOW(),
    updated_at TIMESTAMPTZ  NOT NULL DEFAULT NOW(),
    UNIQUE(locale, namespace, key)
);

CREATE INDEX idx_trans_locale
    ON translations(locale);
CREATE INDEX idx_trans_locale_ns
    ON translations(locale, namespace);
