CREATE TABLE IF NOT EXISTS user (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id      INTEGER REFERENCES project(id) ON DELETE CASCADE,
    name            TEXT,
    full_name       TEXT,
    password        TEXT,
    is_admin        INTEGER
);
