CREATE TABLE IF NOT EXISTS camera (
    id              INTEGER PRIMARY KEY AUTOINCREMENT,
    project_id      INTEGER REFERENCES project(id) ON DELETE CASCADE,
    name            TEXT        DEFAULT "-NA-",
    desc_name       TEXT        DEFAULT "-NA-",
    latitude        TEXT        DEFAULT "-NA-",
    longitude       TEXT        DEFAULT "-NA-",
    altitude        TEXT        DEFAULT "-NA-",
    orientation     TEXT        DEFAULT "-NA-",
    info            TEXT        DEFAULT "-NA-",
    created_on      DATETIME    DEFAULT CURRENT_TIMESTAMP,
    created_by      INTEGER     REFERENCES user(id),
    updated_on      DATETIME    DEFAULT CURRENT_TIMESTAMP,
    updated_by      INTEGER     REFERENCES user(id)
);
