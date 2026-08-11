#!/bin/sh
# Wait for PostgreSQL then run migrations and start.

echo "[pgadmin] Waiting for PostgreSQL ..."
until pg_isready \
    -h "$PGHOST" -p "${PGPORT:-5432}" -q 2>/dev/null
do
    sleep 1
done
echo "[pgadmin] PostgreSQL ready"

echo "[pgadmin] Running migrations ..."
for f in /app/migrations/*.sql; do
    echo "[pgadmin] Applying $(basename "$f") ..."
    PGPASSWORD="$PGPASSWORD" psql \
        -h "$PGHOST" -p "${PGPORT:-5432}" \
        -U "$PGUSER" -d "$PGDATABASE" \
        -f "$f" -q 2>&1 | grep -v "already exists"
done

echo "[pgadmin] Starting server on :${PG_PORT:-5060}"
exec pgadmin-server
