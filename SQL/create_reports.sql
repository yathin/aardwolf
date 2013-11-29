CREATE TABLE IF NOT EXISTS reports (
    id          INTEGER     PRIMARY KEY AUTOINCREMENT,
    name        TEXT        DEFAULT "-NA-",
    query       TEXT        DEFAULT "SELECT * FROM project LIMIT 1",
    read_only   INTEGER     DEFAULT 0,
    created_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by  INTEGER     REFERENCES user(id),
    updated_on  DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by  INTEGER     REFERENCES user(id)
);
