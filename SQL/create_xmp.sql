CREATE TABLE IF NOT EXISTS xmp (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    name            TEXT,
    display_name    TEXT,
    created_on      DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by      INTEGER     REFERENCES user(id),
    updated_on      DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by      INTEGER     REFERENCES user(id)
);
